// I2Cdev library collection - MS5611 I2C device class header file
// Based on the Measurement Specialities MS5611-01BA01 datasheet
// www.meas-spec.com DA5611-01BA01_006 Jul. 19, 2011
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// 16/11/2016 by Emilio Borges <emilio.j.borges@gmail.com>
//
// Changelog:
//     2016-11-16 - initial release

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

#ifndef _MS5611_H_
#define _MS5611_H_

#include "I2Cdev.h"

#define MS5611_ADDRESS_CSB_LOW      0x77 // CSB pin low (GND), default for InvenSense evaluation board and GY-86 board
#define MS5611_ADDRESS_CSB_HIGH     0x76 // CSB pin high (VCC)
#define MS5611_DEFAULT_ADDRESS      MS5611_ADDRESS_CSB_LOW

// better temperature and pressure accuracy, may run a bit slower, highly recommended if temp below 20 C
//#define SECOND_ORDER_TEMP_COMPENSATION

//#define DEFINE_CONSTANTS
#ifdef DEFINE_CONSTANTS
    #define P_MIN_MBAR 1000
    #define P_MAX_MBAR 120000
    #define T_MIN_C (-4000)
    #define T_MAX_C 8500
    #define T_REF_C 2000
#endif

class MS5611 {
public:
    MS5611();
    MS5611(uint8_t address);

    void initialize();
    bool testConnection();

    // I2C commands
    void reset();
    void readPROM();
    void D1Conversion();
    void D2Conversion();
    void readADCResult();

    // calibration data from PROM
    uint16_t C1;    // Pressure sensitivity | SENS_T1
    uint16_t C2;    // Pressure offset | OFF_T1
    uint16_t C3;    // Temperature coefficient of pressure sensitivity | TCS
    uint16_t C4;    // Temperature coefficient of pressure offset | TCO
    uint16_t C5;    // Reference temperature | T_REF
    uint16_t C6;    // Temperature coefficient of the temperature | TEMPSENS

    // digital pressure and temperature data
    uint32_t D1;    // Digital pressure value
    uint32_t D2;    // Digital temperature value

    // temperature calculation variables
    //int32_t dT;         // Difference between actual and reference temperature
    //int32_t TEMP;       // Actual temperature (-40...85C with 0.01C resolution)
    //float TEMP_float;   // -4000 <= TEMP <= 8500; ex. 2007 = 20.07 C

    // pressure calculation variables
    //int64_t OFF;    // Offset at actual temperature
    //int64_t SENS;   // Sensitivity at actual temperature
    //int32_t P;      // Temperature compensated pressure (10...1200 mbar with 0.01 mbar resolution)
    //float P_float;  // 1000 <= P <= 120000; ex. 100009 = 1000.09 mbar

    // calculation functions
    int32_t getTempDifference_dT(); // Difference between actual and reference temperature; -16776960 <= dT <= 16777216
    int32_t getTemperature_int();   // Actual temperature (-40...85C with 0.01C resolution) -4000 <= TEMP <= 8500;
    float getTemperature_float();   // Actual temperature (-40...85C with 0.01C resolution) -40.00 <= TEMP <= 85.00;
    int64_t getTempOffset();        // Offset at actual temperature; -8589672450 <= OFF <= 12884705280
    int64_t getTempSensitivity();   // Sensitivity at actual temperature; -4294836225 <= OFF <= 6442352640
    int32_t getPressure_int();      // Temperature compensated pressure (1000 to 120000)
    float getPressure_float();      // Temperature compensated pressure (10.00 to 1200.00 mbar with 0.01 resolution)


private:
    uint8_t devAddr;

    #ifndef DEFINE_CONSTANTS
    const int32_t P_MIN_MBAR = 1000;
    const int32_t P_MAX_MBAR = 120000;
    const int32_t T_MIN_C = -4000;
    const int32_t T_MAX_C = 8500;
    const int32_t T_REF_C = 2000;
    #endif
};

#endif //_MS5611_H_
