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

#define MS5611_ADDRESS_CSB_LOW      0x77 // CSB pin low (GND), default for default for InvenSense evaluation board and GY-86
#define MS5611_ADDRESS_CSB_HIGH     0x76 // CSB pin high (VCC)
#define MS5611_DEFAULT_ADDRESS      MS5611_ADDRESS_CSB_LOW

#define xxx 0

class MS5611 {
public:
    MS5611();
    MS5611(uint8_t address);

    void initialize();
    bool testConnection();

private:
    uint8_t devAddr;
};

#endif //_MS5611_H_
