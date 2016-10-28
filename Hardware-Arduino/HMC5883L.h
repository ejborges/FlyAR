

#ifndef HMC5883L_H
#define HMC5883L_H

#define HMC5883L_I2C_ADDRESS      0x1E

// Register names according to the datasheet and their address
#define HMC5883L_CONFIG_A         0x00  // R/W
#define HMC5883L_CONFIG_B         0x01  // R/W
#define HMC5883L_MODE             0x02  // R/W
#define HMC5883L_DATA_OUT_X_MSB   0x03  // R
#define HMC5883L_DATA_OUT_X_LSB   0x04  // R
#define HMC5883L_DATA_OUT_Z_MSB   0x05  // R
#define HMC5883L_DATA_OUT_Z_LSB   0x06  // R
#define HMC5883L_DATA_OUT_Y_MSB   0x07  // R
#define HMC5883L_DATA_OUT_Y_LSB   0x08  // R
#define HMC5883L_STATUS           0x09  // R
#define HMC5883L_ID_A             0x0A  // R
#define HMC5883L_ID_B             0x0B  // R
#define HMC5883L_ID_C             0x0C  // R

#endif
