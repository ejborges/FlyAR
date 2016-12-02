// I2Cdev library collection - MS5611 I2C device class header file
// Based on the Measurement Specialities MS5611-01BA01 datasheet
// www.meas-spec.com DA5611-01BA01_006 Jul. 19, 2011
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Parts of this file include code from the AeroQuad MS5611 library cited as [AeroQuad source]
// https://github.com/AeroQuad/AeroQuad/blob/master/Libraries/AQ_BarometricSensor/BarometricSensor_MS5611.h
//
// 2016-11-30 by Emilio Borges <emilio.j.borges@gmail.com>
//
// Changelog:
//     2016-11-30 - initial release
//
// How to use:
// 1. in your working scope (e.g. global scope), create an ms5611 object by calling class constructor
// 2. in your setup(),
//     2.1 call initialize()
//     2.2 (optional) call testConnection()
// 3. in your loop(),
//     3.1 if need temperature only,
//         3.1.1 call initiateD2Conversion()
//         3.1.2 wait approx. 9ms for readADCResult() to return true (you can run other code in the mean time)
//         3.1.3 call getTemperature_float() (or _int() version) (see function description for details)
//     3.2 if need pressure,
//         3.1.1 call initiateD1Conversion()
//         3.1.2 wait approx. 9ms for readADCResult() to return true (you can run other code in the mean time)
//         3.1.3 call initiateD2Conversion()
//         3.1.4 wait approx. 9ms for readADCResult() to return true (you can run other code in the mean time)
//         3.1.5 call getPressure_float() (or _int() version) (see function description for details)
//         3.1.6 (optional) call getTemperature_float() (or _int() version) (see function description for details)
//
// NOTE: may run into runtime errors if 70+ minutes pass between initiateDxConversion() and readADCResult() calls
//       due to Arduino's micros() function reverting to zero after approx. 70 minutes.
// NOTE: enable #define SECOND_ORDER_TEMP_COMPENSATION for better temperature and pressure accuracy.

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2016 Emilio Borges

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include "MS5611.h"

/** Default constructor, uses default I2C address.
 * @see MS5611_DEFAULT_ADDRESS
 */
MS5611::MS5611() {
    MS5611::devAddr = MS5611_DEFAULT_ADDRESS;
    MS5611::MS5611_initialized = false;
}

/** Specific address constructor.
 * @param address I2C address
 * @see MS5611_DEFAULT_ADDRESS
 * @see MS5611_ADDRESS_CSB_LOW
 * @see MS5611_ADDRESS_CSB_HIGH
 */
MS5611::MS5611(uint8_t address) {
    MS5611::devAddr = address;
    MS5611::MS5611_initialized = false;
}

/** Power on and prepare for general usage.
 * See the datasheet for mor information.
 * @return error code
 *
 * Errors:
 * 0 - No errors
 * 1 - Failed device reset (failed to write I2C byte)
 * 2 - Failed PROM read
 */
uint8_t MS5611::initialize() {
    uint8_t error_code = 0;
    for(int attempt = 2; attempt >= 0; --attempt){
        if(!(MS5611::reset())) {error_code = 1; continue;}
        delay(3); // wait 3ms for device to reload internal PROM registers
        if(MS5611::readPROM()) break;
        else error_code = 2;
        if(attempt == 0) {
            MS5611::init_error = true;
            MS5611::MS5611_initialized = false;
            return error_code;
        }
        delay(3);
    }

    MS5611::D1 = 0;
    MS5611::D2 = 0;
    MS5611::conversion_start_time = 0;
    MS5611::reading_D1_conversion = true;
    MS5611::ADC_conversion_in_progress = false;
    MS5611::init_error = false;
    MS5611::MS5611_initialized = true;
    return 0;
}

/** Verify the I2C connection and determine if pressure reading is reasonable (within bounds of 10.00 and 1200.00 mbar)
 * Make sure the device is connected and responds as expected.
 * @return error code
 *
 * Errors:
 * 0 - No errors
 * 1 - Initialization error or device was not initialized
 * 2 - Failed to initiate D1 conversion
 * 3 - Failed to initiate D2 conversion
 * 4 - Calculated pressure value out of bounds
 */
uint8_t MS5611::testConnection() {
    if(MS5611::init_error || !(MS5611::MS5611_initialized)) return 1;
    
    if(!(initiateD1Conversion(MS5611_OSR_256))) return 2;
    while(!(readADCResult())){}
    if(!(initiateD2Conversion(MS5611_OSR_256))) return 3;
    while(!(readADCResult())){}
    int32_t testPressure = getPressure_int();
    if((testPressure < MS5611_P_MIN_MBAR) || (testPressure > MS5611_P_MAX_MBAR)) return 4;

    return 0;
}

/** Write single byte to MS5611
 * @param devAddr I2C slave device address
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool MS5611::writeByte(uint8_t devAddr, uint8_t data){

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") writing byte...");
    #endif
    uint8_t status = 0;
    #if ((I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO < 100) || I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_NBWIRE)
        Wire.beginTransmission(devAddr);
        Wire.send((uint8_t) data); // send data
        Wire.endTransmission();
    #elif (I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO >= 100)
        Wire.beginTransmission(devAddr);
        Wire.write((uint8_t) data); // send data
        status = Wire.endTransmission();
    #elif (I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE)
        Fastwire::beginTransmission(devAddr);
        Fastwire::write(data);
        Fastwire::stop();
    #endif
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.println(". Done.");
    #endif
    return status == 0;
}

/** Read single byte from MS5611
 * @param devAddr I2C slave device address
 * @param data Container for byte value read from device
 * @return Number of bytes read (-1 indicates failure)
 */
int8_t MS5611::readByte(uint8_t devAddr, uint8_t *data){
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") reading byte...");
    #endif

    int8_t count = 0;

    #if (I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE)
        #if (ARDUINO < 100)
            // Arduino v00xx (before v1.0), Wire library
            Wire.beginTransmission(devAddr);
            Wire.endTransmission();
            Wire.beginTransmission(devAddr);
            Wire.requestFrom(devAddr, 1);

            if(Wire.available()) {
                *data = Wire.receive();
                #ifdef I2CDEV_SERIAL_DEBUG
                    Serial.print(*data, HEX);
                #endif
                count++;
            }
            Wire.endTransmission();
        #elif (ARDUINO == 100)
            // Arduino v1.0.0, Wire library
            // Adds standardized write() and read() stream methods instead of send() and receive()
            Wire.beginTransmission(devAddr);
            Wire.endTransmission();
            Wire.beginTransmission(devAddr);
            Wire.requestFrom(devAddr, 1);

            if(Wire.available()) {
                *data = Wire.read();
                #ifdef I2CDEV_SERIAL_DEBUG
                    Serial.print(*data, HEX);
                #endif
                count++;
            }
            Wire.endTransmission();
        #elif (ARDUINO > 100)
            // Arduino v1.0.1+, Wire library
            // Adds official support for repeated start condition, yay!
            Wire.beginTransmission(devAddr);
            Wire.endTransmission();
            Wire.beginTransmission(devAddr);
            Wire.requestFrom(devAddr, 1);

            if(Wire.available()) {
                *data = Wire.read();
                #ifdef I2CDEV_SERIAL_DEBUG
                    Serial.print(*data, HEX);
                #endif
                count++;
            }
        #endif

    #elif (I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE)
        // Fastwire library
        // no loop required for fastwire
        uint8_t status = Fastwire::readBuf(devAddr << 1, devAddr, &data, 1);
        if (status == 0) count = 1;  // success
        else             count = -1; // error
    #endif

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print(". Done (");
        Serial.print(count, DEC);
        Serial.println(" read).");
    #endif

    return count;
}

/** Read two bytes from MS5611
 * @param devAddr I2C slave device address
 * @param data Container for word value read from device
 * @return Number of words read (-1 indicates failure)
 */
int8_t MS5611::readWord(uint8_t devAddr, uint16_t *data){
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") reading word...");
    #endif

    int8_t count = 0;

    #if (I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE)
        #if (ARDUINO < 100)
            // Arduino v00xx (before v1.0), Wire library
            Wire.beginTransmission(devAddr);
            Wire.endTransmission();
            Wire.beginTransmission(devAddr);
            Wire.requestFrom(devAddr, 2); // this wants bytes, 1 word = 2 bytes

            if(Wire.available()) *data = Wire.receive() << 8; // first byte is bits 15-8 (MSb=15)
            if(Wire.available()) *data |= Wire.receive();    // second byte is bits  7-0 (LSb=0)
            #ifdef I2CDEV_SERIAL_DEBUG
                Serial.print(*data, HEX);
            #endif
            count++;
            Wire.endTransmission();
        #elif (ARDUINO == 100)
            // Arduino v1.0.0, Wire library
            // Adds standardized write() and read() stream methods instead of send() and receive()
            Wire.beginTransmission(devAddr);
            Wire.endTransmission();
            Wire.beginTransmission(devAddr);
            Wire.requestFrom(devAddr, 2); // this wants bytes, 1 word = 2 bytes

            if(Wire.available()) *data = Wire.read() << 8; // first byte is bits 15-8 (MSb=15)
            if(Wire.available()) *data |= Wire.read();    // second byte is bits  7-0 (LSb=0)
            #ifdef I2CDEV_SERIAL_DEBUG
                Serial.print(*data, HEX);
            #endif
            count++;
            Wire.endTransmission();
        #elif (ARDUINO > 100)
            // Arduino v1.0.1+, Wire library
            // Adds official support for repeated start condition, yay!
            Wire.beginTransmission(devAddr);
            Wire.endTransmission();
            Wire.beginTransmission(devAddr);
            Wire.requestFrom(devAddr, 2); // this wants bytes, 1 word = 2 bytes

            if(Wire.available()) *data = Wire.read() << 8; // first byte is bits 15-8 (MSb=15)
            if(Wire.available()) *data |= Wire.read();    // second byte is bits  7-0 (LSb=0)
            #ifdef I2CDEV_SERIAL_DEBUG
                Serial.print(*data, HEX);
            #endif
            count++;
            Wire.endTransmission();
        #endif

    #elif (I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE)
        // Fastwire library
        // no loop required for fastwire
        uint8_t intermediate[2];
        uint8_t status = Fastwire::readBuf(devAddr << 1, devAddr, intermediate, 2);
        if (status == 0) {
            count = 1; // success
            *data = ((uint16_t)intermediate[0] << 8) | intermediate[1];
        }
        else count = -1; // error
    #endif

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print(". Done (");
        Serial.print(count, DEC);
        Serial.println(" read).");
    #endif

    return count;
}

/** The Reset sequence shall be sent once after power-on to make sure that
 * the calibration PROM gets loaded into the internal register.
 * It can be also used to reset the device ROM from an unknown condition
 * NOTE: requires at least 3ms to reload registers before next command
 * @return Status of operation (true = success)
 */
bool MS5611::reset(){
    return MS5611::writeByte(MS5611::devAddr, MS5611_RESET);
}

/** Calculate 4-bit CRC from 128-bit PROM
 * @return 4-bit CRC inside a 16-bit container
 */
uint16_t MS5611::MS5611_PROM_CRC4(){
    uint16_t remainder = 0;
    uint16_t original_crc = MS5611::MS5611_PROM[7]; // PROM[7] has to be set to 0 to properly calculate crc4,
    // save now to restore later

    MS5611::MS5611_PROM[7] = (0xFF00 & (MS5611::MS5611_PROM[7]));  // set crc byte to 0

    for(int i = 0; i < 16; ++i){     // operation performed on bytes, 8 words = 16 bytes
        if(i%2) remainder ^= (MS5611::MS5611_PROM[i >> 1]) & 0x00FF;
        else    remainder ^= MS5611::MS5611_PROM[i >> 1] >> 8;

        for(int bit = 8; bit > 0; --bit){
            if(remainder & 0x8000) remainder = (remainder << 1) ^ 0x3000;
            else                   remainder <<= 1;
        }
    }

    MS5611::MS5611_PROM[7] = original_crc;
    return (remainder >> 12) & 0xF; // return calculated CRC4 value
}

/** The read command for PROM shall be executed once after reset by the user to read the content of the
 * calibration PROM and to calculate the calibration coefficients. There are in total 8 addresses resulting in a total
 * memory of 128 bit. Address 0 contains factory data and the setup, addresses 1-6 calibration coefficients and
 * address 7 contains the serial code and CRC.
 * This function reads MS5611 PROM and stores calibration data into C1, C2, ... variables
 * @return Status of operation (true = success)
 */
bool MS5611::readPROM(){
    MS5611::prom_error = 0;
    for(int i = 0; i < MS5611_PROM_REG_COUNT; ++i){
        if(!(MS5611::writeByte(MS5611::devAddr, MS5611_PROM_BASE_ADDR + (i << 1))))
            if(MS5611::readWord(MS5611::devAddr, &(MS5611::MS5611_PROM[i])) < 1) {
                MS5611::prom_error = (20 + i);
                return false;
            }
        else {MS5611::prom_error = (10 + i); return false;}
    }

    MS5611::C1 = MS5611::MS5611_PROM[1];
    MS5611::C2 = MS5611::MS5611_PROM[2];
    MS5611::C3 = MS5611::MS5611_PROM[3];
    MS5611::C4 = MS5611::MS5611_PROM[4];
    MS5611::C5 = MS5611::MS5611_PROM[5];
    MS5611::C6 = MS5611::MS5611_PROM[6];

    uint16_t crc = MS5611::MS5611_PROM_CRC4();
    if(crc != (MS5611::MS5611_PROM[7] & 0xF)) {MS5611::prom_error = 3; return false;}
    else return true;
    //return ((MS5611::MS5611_PROM[7] & 0xF) == MS5611::MS5611_PROM_CRC4());
}
/** Errors:
 * 0 - No error
 * 1x - Failed to write I2C byte command to request PROM word x
 * 2x - Failed to read PROM word x
 * 3  - Calculated CRC4 does not match CRC4 read from device
 */
uint8_t MS5611::getPromError() {
    return MS5611::prom_error;
}

/** Instruct MS5611 to prepare a new uncompensated digital pressure value (D1) based on current environmental reading.
 * @param osr value desired for conversion (see OSR #defines in header file)
 * @return Status of operation (true = success)
 */
bool MS5611::initiateD1Conversion(uint8_t osr){
    if(MS5611::ADC_conversion_in_progress) return false;

         if(osr == MS5611_OSR_4096) MS5611::osr_adc_time_us = MS5611_OSR_4096_ADC_TIME_uS;
    else if(osr == MS5611_OSR_2048) MS5611::osr_adc_time_us = MS5611_OSR_2048_ADC_TIME_uS;
    else if(osr == MS5611_OSR_1024) MS5611::osr_adc_time_us = MS5611_OSR_1024_ADC_TIME_uS;
    else if(osr == MS5611_OSR_512)  MS5611::osr_adc_time_us = MS5611_OSR_512_ADC_TIME_uS;
    else if(osr == MS5611_OSR_256)  MS5611::osr_adc_time_us = MS5611_OSR_256_ADC_TIME_uS;
    else return false;

    if(!(MS5611::writeByte(MS5611::devAddr, MS5611_D1_CONVERSION + osr))) return false;
    MS5611::conversion_start_time = micros();

    MS5611::reading_D1_conversion = true;
    MS5611::ADC_conversion_in_progress = true;
    return true;
}

/** Instruct MS5611 to prepare a new uncompensated digital temperature value (D2) based on current environmental reading.
 * @param osr value desired for conversion (see OSR #defines in header file)
 * @return Status of operation (true = success)
 */
bool MS5611::initiateD2Conversion(uint8_t osr){
    if(MS5611::ADC_conversion_in_progress) return false;

         if(osr == MS5611_OSR_4096) MS5611::osr_adc_time_us = MS5611_OSR_4096_ADC_TIME_uS;
    else if(osr == MS5611_OSR_2048) MS5611::osr_adc_time_us = MS5611_OSR_2048_ADC_TIME_uS;
    else if(osr == MS5611_OSR_1024) MS5611::osr_adc_time_us = MS5611_OSR_1024_ADC_TIME_uS;
    else if(osr == MS5611_OSR_512)  MS5611::osr_adc_time_us = MS5611_OSR_512_ADC_TIME_uS;
    else if(osr == MS5611_OSR_256)  MS5611::osr_adc_time_us = MS5611_OSR_256_ADC_TIME_uS;
    else return false;

    if(!(MS5611::writeByte(MS5611::devAddr, MS5611_D2_CONVERSION + osr))) return false;
    MS5611::conversion_start_time = micros();

    MS5611::reading_D1_conversion = false;
    MS5611::ADC_conversion_in_progress = true;
    return true;
}

/** Reads the 24-bit D1 or D2 measurement from MS5611
 * MUST have called initiateD1Conversion() or initiateD2Conversion() before calling this function!!
 *
 * If the conversion is not executed before the ADC read command, or the ADC read command is
 * repeated, it will give 0 as the output result (D1 or D2 will equal zero). If the ADC read
 * command is sent during conversion the result will be 0, the conversion will not stop and
 * the final result will be wrong. Conversion sequence sent during the already started conversion
 * process will yield incorrect result as well.
 *
 * The conversion process takes approx. 8.22ms to complete (for OSR=4096), wait MS5611_OSR_ADC_TIME_uS before ADC
 * read command. You can run other code while you wait, just make sure to check back to see if conversion is done
 * by calling this function and checking if return==true. When return==true, follow steps to calculate desired
 * measurement.
 *
 * @return Status of operation (true = success)
 */
bool MS5611::readADCResult(){
    if(!(MS5611::ADC_conversion_in_progress) || (micros() < ((MS5611::conversion_start_time + MS5611::osr_adc_time_us) % (1ULL << 32))))
        return false;

    MS5611::ADC_conversion_in_progress = false;

    if(MS5611::writeByte(MS5611::devAddr, MS5611_ADC_READ)){
        MS5611::readByte(MS5611::devAddr, MS5611::conversion_23_16);
        MS5611::readByte(MS5611::devAddr, MS5611::conversion_15_8);
        MS5611::readByte(MS5611::devAddr, MS5611::conversion_7_0);
    }

    if(MS5611::reading_D1_conversion)
         MS5611::D1 = (MS5611::conversion_23_16 << 16) | (MS5611::conversion_15_8 << 8) | MS5611::conversion_7_0;
    else MS5611::D2 = (MS5611::conversion_23_16 << 16) | (MS5611::conversion_15_8 << 8) | MS5611::conversion_7_0;

    return true;
}

/** Calculate the difference between actual and reference temperature
 * @return signed integral temperature difference value between -16776960 and 16777216
 */
int32_t MS5611::getTempDifference_dT(){
    // dT = D2 - T_REF = D2 - C5*(2^8)

    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8); // = D2 - T_REF = D2 - C5*(2^8)
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;
    return dT;
}

/** Calculate temperature in celsius from given PROM calibration and digital temperature data.
 * @return signed integral temperature value between -4000 <= TEMP <= 8500; ex. 2007 = 20.07 C
 */
int32_t MS5611::getTemperature_int() {
    //   dT = D2 - T_REF = D2 - C5*(2^8)
    // TEMP = (20 C) + dT * TEMPSENS = 2000 + (dT * C6)/(2^23)
    //      = 2000 + ((D2 - C5*(2^8)) * C6)/(2^23)

    #ifndef SECOND_ORDER_TEMP_COMPENSATION
    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    return (2000 + ((dT * MS5611::C6) >> 23));

    #else

    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    float TEMP = (2000 + ((dT * MS5611::C6) >> 23)) / 100.0;
    float T2 = 0.0;
    if(TEMP < 20.00) T2 = (dT * dT) / ((uint32_t)1 << 31);
    return (int32_t)(TEMP - T2);
    #endif
}

/** Calculate temperature in celsius from given PROM calibration and digital temperature data.
 * @return signed float temperature value between -40.00 <= TEMP <= 85.00 C
 */
float MS5611::getTemperature_float() {
    //   dT = D2 - T_REF = D2 - C5*(2^8)
    // TEMP = (20 C) + dT * TEMPSENS = 2000 + (dT * C6)/(2^23)
    //      = 2000 + ((D2 - C5*(2^8)) * C6)/(2^23)

    #ifndef SECOND_ORDER_TEMP_COMPENSATION
    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    return (2000 + ((dT * MS5611::C6) >> 23)) / 100.0;

    #else

    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    float TEMP = (2000 + ((dT * MS5611::C6) >> 23)) / 100.0;
    float T2 = 0.0;
    if(TEMP < 20.00) T2 = (dT * dT) / ((uint32_t)1 << 31);
    return TEMP - T2;
    #endif

    // https://github.com/AeroQuad/AeroQuad/blob/master/Libraries/AQ_BarometricSensor/BarometricSensor_MS5611.h
    // return ((1<<5)*2000 + (((D2 - ((int64_t)C5 << 8)) * C6) >> (23-5))) / ((1<<5) * 100.0);
}

/** Calculate offset at actual temperature from given PROM calibration and digital temperature data.
 * @return signed 64-bit integral value between -8589672450 and 12884705280
 */
int64_t MS5611::getTempOffset(){
    // OFF = OFF_T1 + TCO * dT
    //     = C2*(2^16) + (C4 * dT)/(2^7)
    //     = C2*(2^16) + (C4 * (D2 - C5*(2^8)))/(2^7)

    #ifndef SECOND_ORDER_TEMP_COMPENSATION
    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    int64_t OFF = (((uint64_t)MS5611::C2 << 16) + ((MS5611::C4 * dT) >> 7));

    #else

    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    int64_t TEMP = 2000 + ((dT * MS5611::C6) >> 23);
    int64_t OFF = (((uint64_t)MS5611::C2 << 16) + ((MS5611::C4 * dT) >> 7));
    int64_t OFF2 = 0;

    if(TEMP < 2000) {
        OFF2 = (5 * ((TEMP - 2000)*(TEMP - 2000))) >> 1;
        if(TEMP < -1500) OFF2 += (7*((TEMP + 1500)*(TEMP + 1500)));
    }

    OFF -= OFF2;
    #endif

         if(OFF < MS5611_OFF_MIN) OFF = MS5611_OFF_MIN;
    else if(OFF > MS5611_OFF_MAX) OFF = MS5611_OFF_MAX;

    return OFF;
}

/** Calculate sensitivity at actual temperature from given PROM calibration and digital temperature data.
 * @return signed 64-bit integral value between -4294836225 and 6442352640
 */
int64_t MS5611::getTempSensitivity(){
    // SENS = SENS_T1 + TCS * dT
    //      = C1*(2^15) + (C3 * dT)/(2^8)
    //      = C1*(2^15) + (C3 * (D2 - C5*(2^8)))/(2^8)

    #ifndef SECOND_ORDER_TEMP_COMPENSATION
    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    int64_t SENS = (((uint64_t)MS5611::C1 << 15) + ((MS5611::C3 * dT) >> 8));

    #else

    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    int64_t TEMP = 2000 + ((dT * MS5611::C6) >> 23);
    int64_t SENS = (((uint64_t)MS5611::C1 << 15) + ((MS5611::C3 * dT) >> 8));
    int64_t SENS2 = 0;

    if(TEMP < 2000) {
        SENS2 = (5 * ((TEMP - 2000)*(TEMP - 2000))) >> 2;
        if(TEMP < -1500) SENS2 += ((11*((TEMP + 1500)*(TEMP + 1500))) >> 1);
    }

    SENS -= SENS2;
    #endif

         if(SENS < MS5611_SENS_MIN) SENS = MS5611_SENS_MIN;
    else if(SENS > MS5611_SENS_MAX) SENS = MS5611_SENS_MAX;

    return SENS;

}

/** Calculate temperature compensated pressure given PROM calibration and digital pressure and temperature data.
 * @return signed integral pressure value between 1000 <= P <= 120000; ex. 100009 = 1000.09 mbar
 */
int32_t MS5611::getPressure_int() {
    // P = D1 * SENS - OFF
    //   = (((D1 * SENS) / (2^21)) - OFF) / (2^15)

    #ifndef SECOND_ORDER_TEMP_COMPENSATION
    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;
    int64_t OFF = (((uint64_t)MS5611::C2 << 16) + ((MS5611::C4 * dT) >> 7));
    int64_t SENS = (((uint64_t)MS5611::C1 << 15) + ((MS5611::C3 * dT) >> 8));

    #else

    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    int64_t TEMP = 2000 + ((dT * MS5611::C6) >> 23);
    int64_t OFF = (((uint64_t)MS5611::C2 << 16) + ((MS5611::C4 * dT) >> 7));
    int64_t OFF2 = 0;
    int64_t SENS = (((uint64_t)MS5611::C1 << 15) + ((MS5611::C3 * dT) >> 8));
    int64_t SENS2 = 0;

    if(TEMP < 2000) {
        OFF2 = (5 * ((TEMP - 2000)*(TEMP - 2000))) >> 1;
        SENS2 = (5 * ((TEMP - 2000)*(TEMP - 2000))) >> 2;
        if(TEMP < -1500) {
            OFF2 += (7*((TEMP + 1500)*(TEMP + 1500)));
            SENS2 += ((11*((TEMP + 1500)*(TEMP + 1500))) >> 1);
        }
    }

    OFF -= OFF2;
    SENS -= SENS2;
    #endif

         if(OFF < MS5611_OFF_MIN) OFF = MS5611_OFF_MIN;
    else if(OFF > MS5611_OFF_MAX) OFF = MS5611_OFF_MAX;
         if(SENS < MS5611_SENS_MIN) SENS = MS5611_SENS_MIN;
    else if(SENS > MS5611_SENS_MAX) SENS = MS5611_SENS_MAX;

    return (((MS5611::D1 * SENS) >> 21) - OFF) >> 15;
}

/** Calculate temperature compensated pressure given PROM calibration and digital pressure and temperature data.
 * @return signed float pressure value between 10.00 <= P <= 1200.00 mbar
 */
float MS5611::getPressure_float() {
    // P = D1 * SENS - OFF
    //   = (((D1 * SENS) / (2^21)) - OFF) / (2^15)

    #ifndef SECOND_ORDER_TEMP_COMPENSATION
    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    int64_t OFF = (((uint64_t)MS5611::C2 << 16) + ((MS5611::C4 * dT) >> 7));
    int64_t SENS = (((uint64_t)MS5611::C1 << 15) + ((MS5611::C3 * dT) >> 8));

    #else

    int32_t dT = MS5611::D2 - ((uint32_t)MS5611::C5 << 8);
         if(dT < MS5611_DT_MIN) dT = MS5611_DT_MIN;
    else if(dT > MS5611_DT_MAX) dT = MS5611_DT_MAX;

    int64_t TEMP = 2000 + ((dT * MS5611::C6) >> 23);
    int64_t OFF = (((uint64_t)MS5611::C2 << 16) + ((MS5611::C4 * dT) >> 7));
    int64_t OFF2 = 0;
    int64_t SENS = (((uint64_t)MS5611::C1 << 15) + ((MS5611::C3 * dT) >> 8));
    int64_t SENS2 = 0;

    if(TEMP < 2000) {
        OFF2 = (5 * ((TEMP - 2000)*(TEMP - 2000))) >> 1;
        SENS2 = (5 * ((TEMP - 2000)*(TEMP - 2000))) >> 2;
        if(TEMP < -1500) {
            OFF2 += (7*((TEMP + 1500)*(TEMP + 1500)));
            SENS2 += ((11*((TEMP + 1500)*(TEMP + 1500))) >> 1);
        }
    }

    OFF -= OFF2;
    SENS -= SENS2;
    #endif

         if(OFF < MS5611_OFF_MIN) OFF = MS5611_OFF_MIN;
    else if(OFF > MS5611_OFF_MAX) OFF = MS5611_OFF_MAX;
         if(SENS < MS5611_SENS_MIN) SENS = MS5611_SENS_MIN;
    else if(SENS > MS5611_SENS_MAX) SENS = MS5611_SENS_MAX;

    return ((((MS5611::D1 * SENS) >> 21) - OFF) >> 15) / 100.0;
}


