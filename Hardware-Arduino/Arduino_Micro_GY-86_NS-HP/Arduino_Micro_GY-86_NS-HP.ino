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
#include "MS5611.h"
#include "FlyAR.h"


#define serial_out

#define MPU6050_INTERRUPT_PIN 0
#define MS5611_INTERRUPT_PIN 1
#define LED_PIN LED_BUILTIN
bool blinkState = false;

// MPU6050 class default I2C address is 0x68
MPU6050 mpu;

// HMC5883L class default I2C address is 0x1E
HMC5883L mag;

// MS5611 class default I2C address is 0x77
MS5611 baro;

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
#define OUTPUT_READABLE_WORLDACCEL

#define OUTPUT_READABLE_MAGNETOMETER
#define OUTPUT_READABLE_HEADING

#define OUTPUT_READABLE_PRESSURE
#define OUTPUT_READABLE_TEMPERATURE

// MPU6050 control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// MPU6050 orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
int16_t gyro[3];        //To store gyro's measures
int16_t mx, my, mz;     //To store magnetometer readings
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// To check MPU6050 DMP frecuency  (it can be changed it the MotionApps v2 .h file)
int time1,time1old;
float frec1;

// HMC5883L values
float heading_uncompensated;          // Simple magnetic heading. (NOT COMPENSATED FOR PITCH AND ROLL)

// MS5611 values
uint8_t baro_error_code;
bool received_D1_conversion = false;
float pressure_mbar = 0.0;
float temperature_c = 0.0;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINES               ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
//volatile bool ms5611Interrupt = false;     // indicates whether MPU interrupt pin has gone low
void dmpDataReady() {
  mpuInterrupt = true;
}
//void ms5611DataReady() {
//  ms5611Interrupt = true;
//}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {

  Wire.begin(); // join i2c bus
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  
  #ifdef serial_out
  Serial.begin(115200);
  #endif

  // wait for ready
  Serial.println(F("\nSend any character to begin:"));
  while (Serial.available() && Serial.read()); // empty buffer
  while (!Serial.available());                 // wait for data
  while (Serial.available() && Serial.read()); // empty buffer again

  #ifdef serial_out
  Serial.println(F("Initializing I2C devices..."));
  #endif
  mpu.initialize();
  baro_error_code = baro.initialize();

  #ifdef serial_out
  if(baro_error_code) {
      Serial.print(F("MS5611 initialization failed with code: "));
      Serial.println(baro_error_code);
  }

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  //Serial.println(baro.testConnection() ? F("MS5611 connection successful") : F("MS5611 connection failed"));
  baro_error_code = baro.testConnection();
  if(baro_error_code) {
      Serial.print(F("MS5611 connection failed with code: "));
      Serial.println(baro_error_code);
  }
  else Serial.println(F("MS5611 connection successful"));


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

  // --------------------------------------------------------------------------------------------------
  // Have to init mag here because dmpInit sets its own slave 0 params, we overwrite those params here
  // configure magnetometer to pass data to MPU6050 automatically

  // disable MPU6050 i2c master mode
  mpu.setI2CMasterModeEnabled(false);

  // enable i2c master bypass mode
  mpu.setI2CBypassEnabled(true);

  mag.initialize();

  // disable i2c master bypass mode
  mpu.setI2CBypassEnabled(false);

  // configure X axis word
  mpu.setSlaveAddress(0, HMC5883L_DEFAULT_ADDRESS | 0x80); // set slave 0 i2c address, 0x80=read mode
  mpu.setSlaveRegister(0, HMC5883L_RA_DATAX_H);// set slave 0 first data register, 0x03 (high byte of x axis)
  mpu.setSlaveEnabled(0, true);// enable slave 0 data transfer
  mpu.setSlaveWordByteSwap(0, false);
  mpu.setSlaveWriteMode(0, false);
  mpu.setSlaveWordGroupOffset(0, false);
  mpu.setSlaveDataLength(0, 2);

  // configure Y axis word
  mpu.setSlaveAddress(1, HMC5883L_DEFAULT_ADDRESS | 0x80); // set slave 1 i2c address, 0x80=read mode
  mpu.setSlaveRegister(1, HMC5883L_RA_DATAY_H);// set slave 1 first data register, 0x03 (high byte of x axis)
  mpu.setSlaveEnabled(1, true);// enable slave 1 data transfer
  mpu.setSlaveWordByteSwap(1, false);
  mpu.setSlaveWriteMode(1, false);
  mpu.setSlaveWordGroupOffset(1, false);
  mpu.setSlaveDataLength(1, 2);

  // configure Z axis word
  mpu.setSlaveAddress(2, HMC5883L_DEFAULT_ADDRESS | 0x80); // set slave 1 i2c address, 0x80=read mode
  mpu.setSlaveRegister(2, HMC5883L_RA_DATAZ_H);// set slave 1 first data register, 0x03 (high byte of x axis)
  mpu.setSlaveEnabled(2, true);// enable slave 1 data transfer
  mpu.setSlaveWordByteSwap(2, false);
  mpu.setSlaveWriteMode(2, false);
  mpu.setSlaveWordGroupOffset(2, false);
  mpu.setSlaveDataLength(2, 2);

  // enable slave 0 delay; hold MPU6050 interrupt until slave 0 data received
  //mpu.setSlaveDelayEnabled(true); // commented out; may interfere with DMP

  // enable MPU6050 i2c master mode
  mpu.setI2CMasterModeEnabled(true);




// make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    #ifdef serial_out
    Serial.println(F("Enabling DMP..."));
    #endif
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    #ifdef serial_out
    Serial.println(F("Enabling MPU6050 interrupt detection..."));
    #endif
    //enable interrupt INT2 (physical pin 20, PD2) connected to the INTA pin from the MPU6050 on the GY-86
    //jump to the dmpDataReady function on rising edge
    attachInterrupt(MPU6050_INTERRUPT_PIN, dmpDataReady, RISING);

    //enable interrupt INT3 (physical pin 21, PD3) connected to the DRDY pin from the MS5611 on the GY-86
    //jump to the ms5611DataReady function on falling edge
    //attachInterrupt(MS5611_INTERRUPT_PIN, ms5611DataReady, FALLING);

    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    #ifdef serial_out
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    #endif
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    #ifdef serial_out
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
    #endif
  }

  // configure LED for output
  pinMode(LED_PIN, OUTPUT);













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
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {
    // other program behavior stuff here
    // .
    // if you are really paranoid you can frequently test in between other
    // stuff to see if mpuInterrupt is true, and if so, "break;" from the
    // while() loop to immediately process the MPU data
    // .

    if(received_D1_conversion && baro.readADCResult()){
        received_D1_conversion = false;
        pressure_mbar = baro.getPressure_float();
        temperature_c = baro.getTemperature_float();
    }
    else{
        baro.initiateD1Conversion(MS5611_OSR_4096);
        if(baro.readADCResult()){
            received_D1_conversion = true;
            baro.initiateD2Conversion(MS5611_OSR_4096);
        }
    }
  }

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    #ifdef serial_out
    Serial.println(F("FIFO overflow!"));
    #endif

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  }
  else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    // Check DMP frecuency
    time1=micros()-time1old;
    time1old=micros();
    frec1=1000000/time1;



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

    Serial.println("______\tx|y\ty|p\tz|r");
    #endif


    #ifdef OUTPUT_READABLE_YAWPITCHROLL
    // display yaw,pitch,roll angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    #ifdef serial_out
    Serial.print("ypr\t");
    Serial.print(ypr[0] * 180/M_PI);
    Serial.print("\t");
    Serial.print(ypr[1] * 180/M_PI);
    Serial.print("\t");
    Serial.println(ypr[2] * 180/M_PI);
    #endif //serial_out
    #endif

    #ifdef OUTPUT_READABLE_WORLDACCEL
    // display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    #ifdef serial_out
    Serial.print("acc:\t");
    Serial.print(aaWorld.x);
    Serial.print("\t");
    Serial.print(aaWorld.y);
    Serial.print("\t");
    Serial.println(aaWorld.z);
    #endif //serial_out
    #endif

    // Get and process information from DMP
//    mpu.dmpGetQuaternion(&q, fifoBuffer);
//    mpu.dmpGetGyro(gyro, fifoBuffer);
//    //    mpu.dmpGetAccel(&aa, fifoBuffer);  //Use this if you want accelerometer measures
//    mpu.dmpGetGravity(&gravity, &q);
//    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
//    //    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);  //Use this to get linear acceleration apart from gravity.
//    //    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);  //NOT RECOMMENDED. Gives you linear acceleration rotated to initial position.

    #ifdef OUTPUT_READABLE_MAGNETOMETER
    //Read magnetometer measures
    mx=mpu.getExternalSensorWord(0);
    my=mpu.getExternalSensorWord(2);
    mz=mpu.getExternalSensorWord(4);
    #ifdef serial_out
    Serial.print("mag:\t");
    Serial.print(mx);
    Serial.print("\t");
    Serial.print(my);
    Serial.print("\t");
    Serial.println(mz);
    #endif //serial_out
    #endif

    #ifdef OUTPUT_READABLE_HEADING
    //Read magnetometer measures
    mx=mpu.getExternalSensorWord(0);
    my=mpu.getExternalSensorWord(2);
    mz=mpu.getExternalSensorWord(4);

    // calculate heading in degrees. 0 degree indicates North
    heading_uncompensated = atan2(my, mx);
    if(heading_uncompensated < 0) heading_uncompensated += 2 * M_PI;

    #ifdef serial_out
    Serial.print("heading:\t");
    Serial.println(heading_uncompensated * 180/M_PI);
    #endif //serial_out
    #endif

    #if defined(OUTPUT_READABLE_PRESSURE) && defined(serial_out)
    Serial.print("pressure:\t");
    Serial.println(pressure_mbar);
    #endif

    #if defined(OUTPUT_READABLE_TEMPERATURE) && defined(serial_out)
    Serial.print("temperature:\t");
    Serial.println(temperature_c);
    #endif

    #ifdef serial_out
    Serial.print("DMP Freq:\t");
    Serial.println(frec1);
    #endif

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
  }






  


}