/* 
 *  Emilio Borges
 *  October - December 2016
 *  University of Toledo
 *  Senior Design Project - FlyAR
 *  
 *  The FlyAR project aims to provide an augmented reality First Person View (FPV) flying experience. As the vehicle
 *  flies through the sky, virtual graphics overlay the FPV camera image. We use the term augmented reality because 
 *  the virtual graphics rendered over the FPV image seem to stay fixed over real world locations. Given a user created
 *  virtual flying environment, the user can fly in an open area with their FPV goggles or monitor, but see virtual 
 *  objects floating in different sections of the sky and believe those virtual objects actualy exist in the real world.
 *  
 *  This code will run on an Arduino Micro (ATmega32U4) and use I2C to communicate with the GY-86 sensor board and LVTTL 
 *  to communicate with the NS-HP. The Arduino Micro will constantly read raw sensor data and, clean up the data through 
 *  post-processing algorithms, store the values in memory, and repeat to update sensor values. An external processor,
 *  the Raspberry Pi 3 in our case, can then request specific or all of these values for their own use.
 *  
 *  GY-86
 *    10 Degrees of Freedom (DOF) sensor board containing 3-axis accelerometer & 3-axis gyro (MPU6050), 
 *    3-axis magnetometer (HMC5883L), and 1-axis baromoter (MS5611). 
 *  
 *  NS-HP
 *    SkyTraQ NS-HP RTK capable GPS/GNSS receiver
 *    NS-HP is a high performance RTK capable GPS / GNSS receiver achieving centimeter-level accuracy relative positioning.
 *    http://navspark.mybigcommerce.com/ns-hp-rtk-capable-gps-gnss-receiver/
 * 
 * Sources:
 *  [1] https://github.com/farrellf/Balancing_Robot_Firmware/blob/6a7c770a52409c4dbbca72b125163df74aa2956c/main.c
 *  [2] https://youtu.be/hvjNaIlHPV0
 *  [3] http://playground.arduino.cc/Main/MPU-6050
 */


#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file
#include "HMC5883L.h"
#include "FlyAR.h"


#define serial_out

// class default I2C address is 0x68
MPU6050 mpu;

// class default I2C address is 0x1E
HMC5883L mag;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// ================================================================
// ===               INTERRUPT DETECTION ROUTINES               ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
volatile bool ms5611Interrupt = false;     // indicates whether MPU interrupt pin has gone low
void dmpDataReady() {
  mpuInterrupt = true;
}
void ms5611DataReady() {
  ms5611Interrupt = true;
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {

  Wire.begin(); // join i2c bus
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  
  #ifdef serial_out
  Serial.begin(115200);
  Serial.println(F("Initializing I2C devices..."));
  #endif
  mpu.initialize();

#ifdef serial_out
  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  //Serial.println(mag.testConnection() ? F("HMC5883L connection successful") : F("HMC5883L connection failed"));

  // wait for ready
  Serial.println(F("\nSend any character to begin DMP programming: "));
  while (Serial.available() && Serial.read()); // empty buffer
  while (!Serial.available());                 // wait for data
  while (Serial.available() && Serial.read()); // empty buffer again

  Serial.println(F("Initializing DMP..."));
#endif

  // load and configure the DMP
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  // If you don't know yours, you can find an automated sketch for this task from:
  //   http://www.i2cdevlib.com/forums/topic/96-arduino-sketch-to-automatically-calculate-mpu6050-offsets/
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // Have to init mag here because dmpInit sets its own slave 0 params, we overwrite those params here
  mag.initialize();

  //enable interrupt INT2 (pin 20, PD2) connected to the INTA pin from the MPU6050 on the GY-86
  //jump to the dmpDataReady function on rising edge
  attachInterrupt(0, dmpDataReady, RISING);

  //enable interrupt INT3 (pin 21, PD3) connected to the DRDY pin from the MS5611 on the GY-86
  //jump to the ms5611DataReady function on falling edge
  attachInterrupt(1, ms5611DataReady, FALLING);






  // --------------------------------------------------------
  // configure the MPU6050 (gyro/accelerometer)
  //
//  Wire.beginTransmission(MPU6050_ADDRESS);
//  //
//  // exit sleep
//  Wire.write(MPU6050_PWR_MGMT_1);   // queue register address
//  Wire.write(0);                    // queue register value
//  Wire.endTransmission(false);      // transmit/write to register and keep connection open
//  //
//  // gyro sample rate = 8kHz / (1 + 7) = 1kHz; same as accelerometer sample rate
//  Wire.write(MPU6050_SMPLRT_DIV);
//  Wire.write(7);
//  Wire.endTransmission(false);
//  //
//  // gyro full scale = +/- 2000dps
//  Wire.write(MPU6050_GYRO_CONFIG);
//  Wire.write(0x18);
//  Wire.endTransmission(false);
//  //
//  // accelerometer full scale = +/- 4g
//  Wire.write(MPU6050_ACCEL_CONFIG);
//  Wire.write(0x08);
//  Wire.endTransmission(false);
//  //
//  // enable INTA interrupt
//  Wire.write(MPU6050_INT_ENABLE);
//  Wire.write(0x01);
//  Wire.endTransmission(false);
//
//  // --------------------------------------------------------
//  // configure the HMC5883L (magnetometer)
//  //
//  // disable i2c master mode
//  Wire.write(MPU6050_USER_CTRL);
//  Wire.write(0x00);
//  Wire.endTransmission(false);
//  //
//  // enable i2c master bypass mode
//  Wire.write(MPU6050_INT_PIN_CFG);
//  Wire.write(0x02);
//  Wire.endTransmission(true);       // release i2c bus
//  //
//  Wire.beginTransmission(HMC5883L_ADDRESS);
//  //
//  // sample rate = 75Hz
//  Wire.write(HMC5883L_CONFIG_A);
//  Wire.write(0x18);
//  Wire.endTransmission(false);
//  //
//  // full scale = +/- 4.0 Gauss
//  Wire.write(HMC5883L_CONFIG_B);
//  Wire.write(0x80);
//  Wire.endTransmission(false);
//  //
//  // continuous measurement mode
//  Wire.write(HMC5883L_MODE);
//  Wire.write(0x00);
//  Wire.endTransmission(true);
//  //
//  Wire.beginTransmission(MPU6050_ADDRESS);
//  //
//  // disable i2c master bypass mode
//  Wire.write(MPU6050_INT_PIN_CFG);
//  Wire.write(0x00);
//  Wire.endTransmission(false);
//  //
//  // enable i2c master mode
//  Wire.write(MPU6050_USER_CTRL);
//  Wire.write(0x20);
//  Wire.endTransmission(false);
//
//  // --------------------------------------------------------
//  // configure the MPU6050 to automatically read the magnetometer
//  //
//  // slave 0 i2c address, read mode
//  Wire.write(MPU6050_I2C_SLV0_ADDR);
//  Wire.write(HMC5883L_ADDRESS | 0x80);
//  Wire.endTransmission(false);
//  //
//  // slave 0 first data register = 0x03 (x axis)
//  Wire.write(MPU6050_I2C_SLV0_REG);
//  Wire.write(0x03);
//  Wire.endTransmission(false);
//  //
//  // slave 0 transfer size = 6 bytes, enabled data transaction
//  Wire.write(MPU6050_I2C_SLV0_CTRL);
//  Wire.write(6 | 0x80);
//  Wire.endTransmission(false);
//  //
//  // enable slave 0 delay until all data received
//  Wire.write(MPU6050_I2C_MST_DELAY_CTRL);
//  Wire.write(1);
//  Wire.endTransmission(true);

  // --------------------------------------------------------
  // configure the MS5611 (barometer)
  //
  /*
  Wire.beginTransmission(MS5611_ADDRESS);
  //
  // reset
  Wire.write(0x1E);
  Wire.write(0);
  Wire.endTransmission(false);
  //
  // start conversion of pressure sensor
  Wire.write(0x48);
  Wire.write(0);
  Wire.endTransmission(true);
  */
}

void loop() {
  

  
  // to view serial on osx terminal, type "screen /dev/cu.usbmodem411 9600" 
  // check usbmodemXXX has the right port number
  // to exit screen, do CTRL-A CTRL-\  
  
  #ifdef serial_out 
    //Serial.print(F("someting"));
    
    //http://stackoverflow.com/a/15559322
    Serial.write(27);          // ESC command
    Serial.print(F("[2J"));    // clear screen command
    Serial.write(27);
    Serial.print(F("[H"));     // cursor to home command
  #endif

}