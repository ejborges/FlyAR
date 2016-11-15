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
#include "FlyAR.h"
#include "MPU6050.h"
#include "HMC5883L.h"


#define serial_out


void setup() {
  
  #ifdef serial_out
    Serial.begin(115200);
  #endif

  //enable interrupt INT2 (pin 20, PD2) connected to the INTA pin from the MPU6050 on the GY-86
  //jump to the MPU6050_ISR function on rising edge
  attachInterrupt(0, MPU6050_ISR, RISING);

  //enable interrupt INT3 (pin 21, PD3) connected to the DRDY pin from the MS5611 on the GY-86
  //jump to the MS5611_ISR function on rising edge
  attachInterrupt(1, MS5611_ISR, FALLING);


  Wire.begin(); // join i2c bus
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

  #ifdef serial_out
    Serial.println(F("Initializing I2C devices..."));
  #endif

  // --------------------------------------------------------
  // configure the MPU6050 (gyro/accelerometer)
  //
  Wire.beginTransmission(MPU6050_ADDRESS);
  //
  // exit sleep
  Wire.write(MPU6050_PWR_MGMT_1);   // queue register address
  Wire.write(0);                    // queue register value
  Wire.endTransmission(false);      // transmit/write to register and keep connection open
  //
  // gyro sample rate = 8kHz / (1 + 7) = 1kHz; same as accelerometer sample rate
  Wire.write(MPU6050_SMPLRT_DIV);
  Wire.write(7);
  Wire.endTransmission(false);
  //
  // gyro full scale = +/- 2000dps
  Wire.write(MPU6050_GYRO_CONFIG);
  Wire.write(0x18);
  Wire.endTransmission(false);
  //
  // accelerometer full scale = +/- 4g
  Wire.write(MPU6050_ACCEL_CONFIG);
  Wire.write(0x08);
  Wire.endTransmission(false);
  //
  // enable INTA interrupt
  Wire.write(MPU6050_INT_ENABLE);
  Wire.write(0x01);
  Wire.endTransmission(false);
  
  // --------------------------------------------------------
  // configure the HMC5883L (magnetometer)
  //
  // disable i2c master mode
  Wire.write(MPU6050_USER_CTRL);
  Wire.write(0x00);
  Wire.endTransmission(false);
  //
  // enable i2c master bypass mode
  Wire.write(MPU6050_INT_PIN_CFG);
  Wire.write(0x02);
  Wire.endTransmission(true);       // release i2c bus
  //
  Wire.beginTransmission(HMC5883L_ADDRESS);
  //
  // sample rate = 75Hz
  Wire.write(HMC5883L_CONFIG_A);
  Wire.write(0x18);
  Wire.endTransmission(false);
  //
  // full scale = +/- 4.0 Gauss
  Wire.write(HMC5883L_CONFIG_B);
  Wire.write(0x80);
  Wire.endTransmission(false);
  //
  // continuous measurement mode
  Wire.write(HMC5883L_MODE);
  Wire.write(0x00);
  Wire.endTransmission(true);
  //
  Wire.beginTransmission(MPU6050_ADDRESS);
  //
  // disable i2c master bypass mode
  Wire.write(MPU6050_INT_PIN_CFG);
  Wire.write(0x00);
  Wire.endTransmission(false);
  //
  // enable i2c master mode
  Wire.write(MPU6050_USER_CTRL);
  Wire.write(0x20);
  Wire.endTransmission(false);

  // --------------------------------------------------------
  // configure the MPU6050 to automatically read the magnetometer
  //
  // slave 0 i2c address, read mode
  Wire.write(MPU6050_I2C_SLV0_ADDR);
  Wire.write(HMC5883L_ADDRESS | 0x80);
  Wire.endTransmission(false);
  //
  // slave 0 first data register = 0x03 (x axis)
  Wire.write(MPU6050_I2C_SLV0_REG);
  Wire.write(0x03);
  Wire.endTransmission(false);
  //
  // slave 0 transfer size = 6 bytes, enabled data transaction
  Wire.write(MPU6050_I2C_SLV0_CTRL);
  Wire.write(6 | 0x80);
  Wire.endTransmission(false);
  //
  // enable slave 0 delay until all data received
  Wire.write(MPU6050_I2C_MST_DELAY_CTRL);
  Wire.write(1);
  Wire.endTransmission(true);

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



// Interrupt service routine for the MPU6050 + HMC5883L
void MPU6050_ISR(void)
{
  
}

// Interrupt service routine for the MS5611
void MS5611_ISR(void)
{
  
}


















// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//
int MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;
 
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);
 
  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);
 
  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);
 
  return (0);  // return : no error
}

// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, error;
 
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);
 
  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);
 
  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);
 
  return (0);         // return : no error
}
 
// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data)
{
  int error;
 
  error = MPU6050_write(reg, &data, 1);
 
  return (error);
}
