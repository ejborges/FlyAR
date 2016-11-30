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

// NOTE: THIS IS ONLY A PARTIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
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

#ifndef _MS5611_H_
#define _MS5611_H_

#include "I2Cdev.h"

// Address
#define MS5611_ADDRESS_CSB_LOW      0x77 // CSB pin low (GND), default for InvenSense evaluation board and GY-86 board
#define MS5611_ADDRESS_CSB_HIGH     0x76 // CSB pin high (VCC)
#define MS5611_DEFAULT_ADDRESS      MS5611_ADDRESS_CSB_LOW

// Commands
#define MS5611_PROM_BASE_ADDR       0xA0
#define MS5611_PROM_REG_COUNT       8     // number of registers in the PROM
#define MS5611_D1_CONVERSION        0x40
#define MS5611_D2_CONVERSION        0x50
#define MS5611_RESET                0x1E
#define MS5611_ADC_READ             0x00

// D1 and D2 result size (bytes)
#define MS5611_D1D2_SIZE            3

// OSR (Over Sampling Ratio) constants
#define MS5611_OSR_256              0x00 // max ADC_TIME_uS = 0610, pressure resolution rms = 0.065 mbar, temperature resolution rms = 0.012 C
#define MS5611_OSR_512              0x02 // max ADC_TIME_uS = 1180, pressure resolution rms = 0.042 mbar, temperature resolution rms = 0.008 C
#define MS5611_OSR_1024             0x04 // max ADC_TIME_uS = 2290, pressure resolution rms = 0.027 mbar, temperature resolution rms = 0.005 C
#define MS5611_OSR_2048             0x06 // max ADC_TIME_uS = 4550, pressure resolution rms = 0.018 mbar, temperature resolution rms = 0.003 C
#define MS5611_OSR_4096             0x08 // max ADC_TIME_uS = 9050, pressure resolution rms = 0.012 mbar, temperature resolution rms = 0.002 C
#define MS5611_OSR_256_ADC_TIME_uS  1600
#define MS5611_OSR_512_ADC_TIME_uS  2200
#define MS5611_OSR_1024_ADC_TIME_uS 3300
#define MS5611_OSR_2048_ADC_TIME_uS 5500
#define MS5611_OSR_4096_ADC_TIME_uS 10000

// better temperature and pressure accuracy, may run a bit slower, highly recommended if temp below 20 C (reduced error)
//#define SECOND_ORDER_TEMP_COMPENSATION

#define DEFINE_MIN_MAX_CONSTANTS
#ifdef DEFINE_MIN_MAX_CONSTANTS
    #define MS5611_P_MIN_MBAR       1000
    #define MS5611_P_MAX_MBAR       120000
    #define MS5611_T_MIN_C          (-4000)
    #define MS5611_T_MAX_C          8500
    #define MS5611_T_REF_C          2000
    #define MS5611_DT_MIN           (-16776960)
    #define MS5611_DT_MAX           16777216
    #define MS5611_OFF_MIN          (-8589672450)
    #define MS5611_OFF_MAX          12884705280
    #define MS5611_SENS_MIN         (-4294836225)
    #define MS5611_SENS_MAX         6442352640
#endif

class MS5611 {
public:
    MS5611();
    MS5611(uint8_t address);

    void initialize();
    bool testConnection();

    // Public I2C commands
    bool reset();
    bool initiateD1Conversion(uint8_t osr);
    bool initiateD2Conversion(uint8_t osr);
    bool readADCResult();

    /** Calculation functions; require PROM calibration data and current ADC conversion for D1 and/or D2.
     *
     * Before calling any of these functions, call initiateDxConversion() first and
     * wait for true return from readADCResult(). After calling initializeDxConversion(),
     * readADCResult will take approx. 9ms to return true. Feel free to run other code in the mean time.
     */
    int32_t getTempDifference_dT(); // Difference between actual and reference temperature; -16776960 <= dT <= 16777216
    int32_t getTemperature_int();   // Actual temperature (-40...85C with 0.01C resolution) -4000 <= TEMP <= 8500;
    float getTemperature_float();   // Actual temperature (-40...85C with 0.01C resolution) -40.00 <= TEMP <= 85.00;
    int64_t getTempOffset();        // Offset at actual temperature; -8589672450 <= OFF <= 12884705280
    int64_t getTempSensitivity();   // Sensitivity at actual temperature; -4294836225 <= OFF <= 6442352640
    int32_t getPressure_int();      // Temperature compensated pressure (1000 to 120000)
    float getPressure_float();      // Temperature compensated pressure (10.00 to 1200.00 mbar with 0.01 resolution)


private:
    uint8_t devAddr;
    uint16_t MS5611_PROM[MS5611_PROM_REG_COUNT];

    // calibration data from PROM
    uint16_t C1;    // Pressure sensitivity | SENS_T1
    uint16_t C2;    // Pressure offset | OFF_T1
    uint16_t C3;    // Temperature coefficient of pressure sensitivity | TCS
    uint16_t C4;    // Temperature coefficient of pressure offset | TCO
    uint16_t C5;    // Reference temperature | T_REF
    uint16_t C6;    // Temperature coefficient of the temperature | TEMPSENS

    // digital pressure and temperature data
    uint32_t D1;    // Digital pressure value (uncompensated)
    uint32_t D2;    // Digital temperature value (uncompensated)

    // working values (to help run MS5611 code)
    unsigned long conversion_start_time;
    uint32_t conversion_23_16;
    uint32_t conversion_15_8;
    uint32_t conversion_7_0;
    uint16_t osr_adc_time_us; // safeguard microsecond time for ADC completion for given OSR value (see OSR #defines)

    // status flags
    bool MS5611_initialized;
    bool reading_D1_conversion;
    bool ADC_conversion_in_progress;
    bool init_error;

    // calculate 4-bit cyclic redundancy check
    uint16_t MS5611_PROM_CRC4();

    // Private I2C commands
    bool readPROM();

    // The MS5611 handle's I2C communication differently than other I2C devices.
    // Here, we don't need to specify a register address, just the device address and data.
    bool writeByte(uint8_t devAddr, uint8_t data);
    int8_t readByte(uint8_t devAddr, uint8_t *data);
    int8_t readWord(uint8_t devAddr, uint16_t *data);

    #ifndef DEFINE_MIN_MAX_CONSTANTS
    const int32_t MS5611_P_MIN_MBAR = 1000;
    const int32_t MS5611_P_MAX_MBAR = 120000;
    const int32_t MS5611_T_MIN_C    = -4000;
    const int32_t MS5611_T_MAX_C    = 8500;
    const int32_t MS5611_T_REF_C    = 2000;
    const int32_t MS5611_DT_MIN     = -16776960;
    const int32_t MS5611_DT_MAX     = 16777216;
    const int64_t MS5611_OFF_MIN    = -8589672450;
    const int64_t MS5611_OFF_MAX    = 12884705280;
    const int64_t MS5611_SENS_MIN   = -4294836225;
    const int64_t MS5611_SENS_MAX   = 6442352640;
    #endif
};

#endif //_MS5611_H_
