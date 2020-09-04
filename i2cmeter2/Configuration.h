/*
Configuration file for power and SWR calculations and calibration. 

WC8C, Dennis Cabell
-----------------------------------------------------------------------
**********************************************************************/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// comment this line out of you want to run hardware serial (wire to pin 0 instead of pin 9)
#define USE_SW_SERIAL

///////////////////////////////////////////////////
// To use a speed other than 9600, you must modify the Raduino CEC firmware and 
// the Nextion formware to use the same speed.
//
// In the CEC firmware, the speed is in the LCDNextion_Init() in the ubitx_lcd_nextion file
//
// For the nextion, in the Nextion editor on the pboot in the Event Postinitialization 
// put baud=<rate> on the first line, for example baud=57600
///////////////////////////////////////////////////
//#define SERIAL_SPEED 57600
#define SERIAL_SPEED 9600

// comment this line out if you dont have the power/swr meter lines connected
#define ENABLE_POWER_SWR_METER

///////////////////////////////
// S-Meter Calibration
///////////////////////////////
#define SMETER_GAIN ((float)1.0)   // lower number to increase the S-Meter reading

///////////////////////////////
// Power SWR meter calibration
///////////////////////////////
#define CAL_REF_VOLTAGE (float)5.074            // Measured on multimeter

#define CAL_FWD_DIODE_DROP  (float)0.245        // Measured on multimeter
#define CAL_REV_DIODE_DROP  (float)0.235        // Measured on multimeter

#define DEFLT_CAL_FWD_ADJUST_MULTIPLIER (float)1.0    // set first by comparing forward power to a calibrated power meter

// change this to calibrate power reading for each band
#define CAL_FWD_ADJUST_MULTIPLIER(freq)  ( \
                                         (freq <  3000000) ? DEFLT_CAL_FWD_ADJUST_MULTIPLIER : \
                                         (freq <  5000000) ? (float).995 : \
                                         (freq <  6000000) ? (float).995 : \
                                         (freq <  8000000) ? (float).995 : \
                                         (freq < 11000000) ? (float).961 : \
                                         (freq < 15000000) ? (float).97 : \
                                         (freq < 19000000) ? (float).95 : \
                                         (freq < 22000000) ? (float).95 : \
                                         (freq < 25000000) ? (float).935 : \
                                         (freq < 30000000) ? (float).92 : \
                                         DEFLT_CAL_FWD_ADJUST_MULTIPLIER )


#define CAL_REV_ADJUST_MULTIPLIER(freq) CAL_FWD_ADJUST_MULTIPLIER(freq)     // set to same as forward, or replace with macro similar to above to customize rev power
                                                                            // set second by comparing to swr or reverse power on a calibrated meter

#endif //POWERSWRCALIBRATION_H
