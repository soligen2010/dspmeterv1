//===================================================================
//Begin of Nextion LCD Protocol
//
// v0~v9, va~vz : Numeric (Transceiver -> Nextion LCD)
// s0~s9  : String (Text) (Transceiver -> Nextion LCD)
// vlSendxxx, vloxxx: Reserve for Nextion (Nextion LCD -> Transceiver)
//
//===================================================================

#ifndef NextionProtocol_h
#define NextionProtocol_h
#include "AltSoftSerial.h"
#include <arduino.h>
#include "Configuration.h"
#include "i2cmeter2.h"

//================================================================
//EEPROM Section
//================================================================
#define EEPROM_DSPTYPE      100
#define EEPROM_SMETER_UART  111

#ifdef USE_SW_SERIAL
  #define SERIAL_OUTPUT altSerial
#else
  #define SERIAL_OUTPUT Serial
#endif

#define MAX_FORWARD_BUFF_LENGTH 30

#define SWS_HEADER_CHAR_TYPE 'c'  //1Byte Protocol Prefix
#define SWS_HEADER_INT_TYPE  'v'  //Numeric Protocol Prefex
#define SWS_HEADER_STR_TYPE  's'  //for TEXT Line compatiable Character LCD Control

#define CMD_NOW_DISP      '0' //c0
#define CMD_VFO_TYPE      'v' //cv
#define CMD_CURR_FREQ     'c' //vc
#define CMD_CURR_MODE     'c' //cc
#define CMD_VFOA_FREQ     'a' //va
#define CMD_VFOA_MODE     'a' //ca
#define CMD_VFOB_FREQ     'b' //vb
#define CMD_VFOB_MODE     'b' //cb
#define CMD_IS_RIT        'r' //cr
#define CMD_RIT_FREQ      'r' //vr
#define CMD_IS_TX         't' //ct
#define CMD_IS_DIALLOCK   'l' //cl
#define CMD_IS_SPLIT      's' //cs
#define CMD_IS_TXSTOP     'x' //cx
#define CMD_TUNEINDEX     'n' //cn
#define CMD_SMETER        'p' //cs
#define CMD_SIDE_TONE     't' //vt
#define CMD_KEY_TYPE      'k' //ck
#define CMD_CW_SPEED      's' //vs
#define CMD_CW_DELAY      'y' //vy
#define CMD_CW_STARTDELAY 'e' //ve
#define CMD_ATT_LEVEL     'f' //vf
#define CMD_IS_IFSHIFT    'i' //ci
#define CMD_IFSHIFT_VALUE 'i' //vi
#define CMD_SDR_MODE      'j' //cj
#define CMD_UBITX_INFO     'm' //cm  Complete Send uBITX Information

// defines the minumum amount of time between commands so that the nextion has time to process the command
// Some data is sent in 2 parts with a 50ms timer in the nextion to process the data so must be > 50 
// if set too low odd things can happen in the nextion
#define LAST_TIME_INTERVAL 75 

//Once Send Data, When boot
//arTuneStep, When boot, once send
//long arTuneStep[5];
#define CMD_AR_TUNE1      '1' //v1
#define CMD_AR_TUNE2      '2' //v2
#define CMD_AR_TUNE3      '3' //v3
#define CMD_AR_TUNE4      '4' //v4
#define CMD_AR_TUNE5      '5' //v5

class NextionProtocol
{
 public:
  
  #ifdef USE_SW_SERIAL
  NextionProtocol(AltSoftSerial& SERIAL_OUTPUT): SERIAL_OUTPUT(SERIAL_OUTPUT) {};
  #endif

  void Initialize();
  void SendHeader(char varType, char varIndex);
  void SendCommandUL(char varIndex, unsigned long sendValue);
  void SendCommandL(char varIndex, long sendValue);
  void SendCommandStr(char varIndex, char* sendValue);
  void SendCommand1Num(char varType, char sendValue); //0~9 : Mode, nowDisp, ActiveVFO, IsDialLock, IsTxtType, IsSplitType
  void SendCommandEnding(char varIndex);
  void SendCommandStrEnding(char varIndex);
  bool SendDecodeCharacters(char *characters);
  int ForwardData(void);
  void SendScaledSMeter(int meterToUartInterval);
  void CalculateScaledSMeter(int ADC_DIFF);
  void SendFFTData(int readSampleCount, int *readArray);
  bool ResponseConfig();
  bool SendPowerSwr(float power, float swr, bool sendSwrAsSmeter = true);
  bool SerialDataToProcess();
  bool LastSendWaitTimeElapsed();
  bool WaitUntilCommandCanBeSent();

  /*
   * Most of these are not used.  Uncomment the ones if they become needed.
   */
  //char L_nowdisp = -1;          //Sended nowdisp
  //char L_vfoActive;             //vfoActive
  unsigned long L_vfoCurr;      //vfoA
  //byte L_vfoCurr_mode;          //vfoA_mode
  //unsigned long L_vfoA;         //vfoA
  //byte L_vfoA_mode;             //vfoA_mode
  //unsigned long L_vfoB;         //vfoB
  //byte L_vfoB_mode;             //vfoB_mode
  //char L_ritOn;
  //unsigned long L_ritTxFrequency; //ritTxFrequency
  //char L_inTx;
  //byte L_isDialLock;            //byte isDialLock
  //byte  L_Split;            //isTxType
  //byte  L_TXStop;           //isTxType
  //byte L_tuneStepIndex;     //byte tuneStepIndex
  //unsigned long L_sideTone; //sideTone
  //byte L_cwKeyType;          //L_cwKeyType 0: straight, 1 : iambica, 2: iambicb
  //unsigned int L_cwSpeed;   //cwSpeed
  //byte L_cwDelayTime;       //cwDelayTime
  //byte L_delayBeforeCWStartTime;  //byte delayBeforeCWStartTime
  //byte L_attLevel;
  //byte L_isIFShift;             //1 = ifShift, 2 extend
  //int L_ifShiftValue;
  //byte L_sdrModeOn;
  //int idleStep = 0;
  byte scaledSMeter = 0;

  char DSPType = 1; //0 : Not Use, 1 : FFT, 2 : Morse Decoder, 3 : RTTY Decoder
  uint8_t SMeterToUartSend       = 0; //0 : Send, 1: Idle
  double coeff;
  uint8_t TXStatus = 0;   //0:RX, 1:TX  
  int magnitudelimit_low = 30;
  bool frequencyHasChanged = true;
  bool nextionIsConnected = false; // This gets set to true as soon as inbound serial data arrives.

protected:
  void CommandParser(char* ForwardBuff, int lastIndex);

  uint8_t cwDecodeHz = 9;
  uint8_t responseCommand = 0;  
  unsigned long lastForwardmili = 0;
  float lastSentSwr = -1;
  float lastSentPower = -1;
  uint8_t LastSendScaledSMeter;
  uint8_t SMeterToUartIdleCount  = 0;
  
  #ifdef USE_SW_SERIAL
    AltSoftSerial &SERIAL_OUTPUT;
  #endif
};

#endif
