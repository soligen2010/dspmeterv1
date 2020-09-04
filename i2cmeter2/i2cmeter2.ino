/*
FFT, CW Decode for uBITX
KD8CEC, Ian Lee
WC8C, Dennis Cabell
Version : 2
-----------------------------------------------------------------------
License : See fftfunctions.cpp for FFT and CW Decode.
**********************************************************************/

#include <Wire.h>
#include <EEPROM.h>
#include "i2cmeter2.h"
#include "AltSoftSerial.h"
#include "CWDecoder.h"
#include "fftfunctions.h"
#include "NextionProtocol.h"
#include "PowerSwr.h"
#include "Configuration.h"


#ifdef USE_SW_SERIAL
  AltSoftSerial SERIAL_OUTPUT;
  NextionProtocol nextion = NextionProtocol(SERIAL_OUTPUT);
#else
  NextionProtocol nextion;
#endif

#ifdef ENABLE_POWER_SWR_METER
  PowerSwr powerSwrCalculator;
#endif

CWDecoder decoder;

int FFTReal[SAMPLESIZE];
int ADC_DIFF  = 0;
volatile byte I2CCommand = 0; // volatile because I2C interrupt changes this

void setup() 
{    
  Wire.begin(I2CMETER_ADDR);              //j : S-Meter Slave Address
  Wire.onReceive(I2CReceiveEvent);        //
  Wire.onRequest(I2CRequestEvent);
  
  Serial.begin(SERIAL_SPEED, SERIAL_8N1);
  Serial.flush();
  
  #ifdef USE_SW_SERIAL
    SERIAL_OUTPUT.begin(SERIAL_SPEED);
  #endif
    
  nextion.Initialize();

  //spend time just forwaring data while everything starts up.  There is a lot of initial data to transfer.
  unsigned long startMillis = millis() + 2000;
  while (millis() < startMillis)
  {
    nextion.ForwardData();
  }
}

//void loop() 
//{
//  static char xxxxx[] = ".--..-..-";
//  decoder.docode(xxxxx);
//  Serial.println(decoder.GetDecodedCharacters());
//  decoder.ClearDecodedCharacters();
//
//  static char xxxx []= "...---...";
//  decoder.docode(xxxx);
//  Serial.println(decoder.GetDecodedCharacters());
//  decoder.ClearDecodedCharacters();
//
//  delay(500000);
//}

void loop() 
{
   nextion.ForwardData();  // this will not return until the input UART is empty and the we are not in the middle of a command

  //===========================================
  //Processing for when a config response has needs to be sent
  //===========================================
  if (nextion.ResponseConfig())
  {
    return; // if anything was sent to nextion, then start loop over so any data that may have come in can be processed.
  }

  //===========================================
  //Processing for when the frequency has changed
  //===========================================
  if (nextion.frequencyHasChanged)
  {
     // TBD: Call process to do when the frequency has changed (e.x. output to an antenna tuner)

     nextion.frequencyHasChanged = false;
     return; //Start a new loop so more serial data can be processed
  }

  //===========================================
  //TRANSCEIVER STATUS : TX
  //===========================================
  if (nextion.TXStatus == 1 || !nextion.nextionIsConnected)  //  TX Mode, or always read if there is no nextion so values are available if requested via I2C
  {
    #ifdef ENABLE_POWER_SWR_METER
      powerSwrCalculator.PowerSwrCalculation(nextion.L_vfoCurr);
      if (nextion.nextionIsConnected)
      {
        nextion.SendPowerSwr(powerSwrCalculator.power, powerSwrCalculator.swr);
      }
    #endif
    return; //Do not process ADC, FFT, Decode Morse or RTTY, only Power and SWR Data Return
  }

  //===========================================
  //TRANSCEIVER STATUS : RX
  //===========================================
  //===========================================
  // ADC Sampling
  //===========================================  
  int new_ADC_DIFF = GrepADC((nextion.DSPType == 2 || nextion.DSPType == 3) ? DECODE_MORSE_SAMPLESIZE : SAMPLESIZE, FFTReal);
  if (new_ADC_DIFF < 0)
  {
    return; // If the sample was not completed, then return so incomming data can be processed in the next loop.
  }
  
  noInterrupts();         // disable interrupts while saving these so that the I2C ISR cant read them while they are changing.
  ADC_DIFF = new_ADC_DIFF;
  interrupts();
  nextion.CalculateScaledSMeter(ADC_DIFF);

  //===================================================================================
  // DSP Routine
  //===================================================================================
  if (nextion.nextionIsConnected)
  {
    if (nextion.DSPType == 0) 
    {
      nextion.SendScaledSMeter(FastMeterToUartInterval);
    }
    else if (nextion.DSPType == 1 && nextion.LastSendWaitTimeElapsed()) //Spectrum : FFT => Send To UART
    {
      nextion.SendScaledSMeter(FFTMeterToUartInterval);
      nextion.SendFFTData(SAMPLESIZE, FFTReal); // if new data comes in, the send gets aborted and control returns so the loop can be started over.
    }
    else if (nextion.DSPType == 2)          //Decode Morse
    {
      decoder.Decode_Morse(GetMagnatude(nextion.coeff, DECODE_MORSE_SAMPLESIZE, FFTReal), nextion.magnitudelimit_low);
      if (!nextion.SerialDataToProcess())  // only send decoded data if nothing new has come in to process.
      {
        if (nextion.SendDecodeCharacters(decoder.GetDecodedCharacters()))
        {
          decoder.ClearDecodedCharacters();  // if send succeeded clear the characters
        }
        nextion.SendScaledSMeter(DecodeMeterToUartInterval);// Note that if characters were sent then this will defer until LAST_TIME_INTERVAL has elapsed.  They wont send back to back
      }
    }
  }
} //end of main loop

int GrepADC(int readSampleCount, int *readArray)
{
  #ifdef USE_SW_SERIAL
    // if there is data to still output then return so normal forwarding can continue.
    // only want to sample if the SW Serial output is empty so interrupts dont disrupt sample timing.
    if (!SERIAL_OUTPUT.TxBufferIsEmpty())
    {
      return -1;
    }
  #endif

  unsigned long currentms = 0;
  unsigned long newCurrentms = 0;
  int ADC_MAX = 0;
  int ADC_MIN = 30000;
  
  analogRead(SIGNAL_METER_ADC);  // initial unused value helps accuracy since ADC may have been switched to read power & swr.
  newCurrentms = micros();

  for(int i=0; i < readSampleCount; i++)
  {
    if (nextion.SerialDataToProcess())
    {
      // if something has come in then terminate the FFT sample so incomming data can be processed.
      // this is so that frequency tuning does no lag when in FFT mode.
      // returning -1 instead of ADC_DIFF indicates that the sample is incolplete.
      return -1;
    }
    
    currentms = newCurrentms;
    readArray[i] = analogRead(SIGNAL_METER_ADC);
    ADC_MAX = max(ADC_MAX, readArray[i]);
    ADC_MIN = min(ADC_MIN, readArray[i]);

    while((newCurrentms - SAMPLE_INTERVAL) < currentms){newCurrentms = micros();}
  } //end of for
  
  return ((float)ADC_MAX - (float)ADC_MIN) / SMETER_GAIN;  // return ADC_DIFF
}

void I2CReceiveEvent(void)
{
  int readCommand = 0; // byte를 읽어 int로 변환  
  
  while(Wire.available() > 0)     // for Last command
  {
     readCommand = Wire.read();
  }

  if (0x50 <= readCommand && readCommand <= 0x59)
  {
    I2CCommand = readCommand;
  }
}

void I2CRequestEvent(void)
{
  byte command = I2CCommand; // make a copy in case it case the other interrupt changes it while this is executing
  
  if (command == I2CMETER_CALCS)
  {
    Wire.write(nextion.scaledSMeter);
  }
  else if (command == I2CMETER_UNCALCS)
  {
    // cast to a long becasue this theoretically can be 1023 * 1023, then cast it back down after the constrain
    // Not sure why this is being squared since it could cause an overflow - maintianing behavior of original code.
    Wire.write((uint8_t)constrain(((long)ADC_DIFF) * ((long)ADC_DIFF), 0, 255));
  }
  /* Notes on power and SWR
   *  
   * Original code sends an int, so this does too, but this will get converted to 8 bits if the value is > 255
   * constraining to 255 seems too small. Ask Dr. Lee
   * 
   * Dont need to read if in RX mode, just send zero because when not transmitting the value should always read zero
   * however if there is no nextion connected (i.e. there is no serial input), then the TX status is unknown
   * so the data needs sent.  
   *
   * Note that the analog reads happen in the main loop and the values are saved so they are available here.
   * This is to keep the interrupt fast because the analogRead is a bit slow.
   */
  else if (command == I2CMETER_CALCP)    // POWER
  {
    int value = 0;
    #ifdef ENABLE_POWER_SWR_METER
      if (nextion.TXStatus == 1 || !nextion.nextionIsConnected)  //TX Mode or there isn't a nextion
      {
        value = powerSwrCalculator.powerIn;
      }
    #endif
    Wire.write(value);  //POWER
  }
  else if (command == I2CMETER_CALCR)        // REV POWER
  {
    int value = 0;
    #ifdef ENABLE_POWER_SWR_METER
      if (nextion.TXStatus == 1 || !nextion.nextionIsConnected)  //TX Mode or there isn't a nextion
      {
        value = powerSwrCalculator.revPowerIn;
      }
    #endif
    Wire.write(value);  //Reverse Power
  }
}
