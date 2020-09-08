/*
Configuration file for Nextion LCD and Control MCU
The parameter can be set according to the CPU used. 

KD8CEC, Ian Lee
-----------------------------------------------------------------------
Aug 2020: Refactored and modifed by WC8C, Dennis Cabell
License: Same as origial code.

**********************************************************************/

#ifndef I2CMETER2_H
#define I2CMETER2_H

#include <arduino.h>

//================================================================
//COMMUNICATION SECTION
//================================================================
#define FastMeterToUartInterval  4
#define FFTMeterToUartInterval  0
#define DecodeMeterToUartInterval  48

//================================================================
//FFT and Decode Morse
//================================================================
#define FFTSIZE 64
#define MAX_FFT_FREQUENCY 3200                             // 3200 gives and even frequency bin size of 50Hz
#define SAMPLE_FREQUENCY (MAX_FFT_FREQUENCY * 2)           //Caution, do not exceed the speed of the GrepADC loop.  Must be at least MAX_FFT_FREQUENCY * 2
#define SAMPLESIZE (int)((long)SAMPLE_FREQUENCY * (long)FFTSIZE / (long)MAX_FFT_FREQUENCY)  // This calculations makes the FFT frequency resolution the same as the FFTSIZE
#define DECODE_MORSE_SAMPLESIZE (SAMPLESIZE * 3 / 8)   // Less frequency resolution when CW decoding is faster and helps the signal be in the decode window.  Approx. 125 hz resolution
#define SAMPLE_INTERVAL  (unsigned long)round(1000000.0 * (1.0 / (float)SAMPLE_FREQUENCY))

#define EEPROM_CW_FREQ      120
#define EEPROM_CW_MAG_LOW   122

//================================================================
//DEFINE for I2C Command
//================================================================
//S-Meter Address
#define I2CMETER_ADDR     0x58  //changed from 0x6A
//VALUE TYPE============================================
//Signal
#define I2CMETER_CALCS    0x59 //Calculated Signal Meter
#define I2CMETER_UNCALCS  0x58 //Uncalculated Signal Meter

//Power
#define I2CMETER_CALCP    0x57 //Calculated Power Meter
#define I2CMETER_UNCALCP  0x56 //UnCalculated Power Meter

//SWR
#define I2CMETER_CALCR    0x55 //Calculated Reversse Power Meter
#define I2CMETER_UNCALCR  0x54 //Uncalculated Reversse Power Meter

#define SIGNAL_METER_ADC  A7
#define POWER_METER_ADC   A3
#define REV_POWER_METER_ADC     A2

#endif // I2CMETER2_H
