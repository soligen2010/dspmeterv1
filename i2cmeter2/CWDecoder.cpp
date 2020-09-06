/*
CW Decode. 
KD8CEC, Ian Lee
-----------------------------------------------------------------------
//The section on CW decode logic is specified in this code.
License : I follow the license of the previous code and I do not add any extra constraints.
I hope that the Comment I made or the Comment of OZ1JHM will be maintained.
**********************************************************************/

/*
 * August 2020.  Modified by WC8C, Dennis Cabell
 * License : Same as previous code.
 */

#include <arduino.h>
#include "CWDecoder.h"

//=====================================================================
//The CW Decode code refers to the site code below.
//https://k2jji.org/2014/09/18/arduino-base-cw-decoder/
//Some code has been modified, but the original comments remain intact.
// code below is optimal for use in Arduino.
//Thanks to OZ1JHM 
//KD8CEC
//=====================================================================

///////////////////////////////////////////////////////////////////////
// CW Decoder made by Hjalmar Skovholm Hansen OZ1JHM  VER 1.01       //
// Feel free to change, copy or what ever you like but respect       //
// that license is http://www.gnu.org/copyleft/gpl.html              //
// Discuss and give great ideas on                                   //
// https://groups.yahoo.com/neo/groups/oz1jhm/conversations/messages //
///////////////////////////////////////////////////////////////////////

char* CWDecoder::GetDecodedCharacters()
{
  return decodedCharacters;
}

void CWDecoder::ClearDecodedCharacters()
{
  decodedCharacters[0] = 0;
}

void CWDecoder::SaveDisplayCharacter(char asciinumber)
{
  if (asciinumber != 0)
  {
    // if decodedCharacters is full make room for a new one
    if (strlen(decodedCharacters) >= MAX_DECODED_CHARACTERS_BUFFER_SIZE)
    {
      for (uint8_t x = 0 ; x < MAX_DECODED_CHARACTERS_BUFFER_SIZE; x++)  
      {
        decodedCharacters[x] = decodedCharacters[x+1];
      }
     }
  
    uint8_t position = strlen(decodedCharacters);
    decodedCharacters[position] = (asciinumber == 3 || asciinumber == 4 || asciinumber == 6) ? 0 : asciinumber;
    decodedCharacters[position + 1] = 0;
  }
}

void CWDecoder::docode(char *code)
{
  char character = getDecodeCharacter(code);
  char character2 = getDecodeCharacter(code); // if there are any symbols left over, get a character.  Returns zero if there are no symbols left;
  if (code[0] != 0)   // if there are still any symbols left, then dont try to continue, print a ? to indicate something unknown
  {
    code[0] = 0;
    SaveDisplayCharacter('?');
  }
  SaveDisplayCharacter(character2);
  SaveDisplayCharacter(character);
 }

char CWDecoder::getDecodeCharacter(char *code)
{
  // match against the end of the string, then remove the symbols that were matched, leaving any extra symbol in the string for next time.

  uint8_t numberOfSymbols = strlen(code);

  if (numberOfSymbols >= 7)
  {
    char* compareCode = &code[numberOfSymbols - 7];
    if (strcmp(compareCode,"...-..-") == 0) {code[numberOfSymbols - 7] = 0; return 36;}
  }
  
  if (numberOfSymbols >= 6)
  {
    char* compareCode = &code[numberOfSymbols - 6];
    if (strcmp(compareCode,"..--..") == 0) {code[numberOfSymbols - 6] = 0; return 63;}
    if (strcmp(compareCode,".-.-.-") == 0) {code[numberOfSymbols - 6] = 0; return 46;}
    if (strcmp(compareCode,"--..--") == 0) {code[numberOfSymbols - 6] = 0; return 44;}
    if (strcmp(compareCode,"-.-.--") == 0) {code[numberOfSymbols - 6] = 0; return 33;}
    if (strcmp(compareCode,".--.-.") == 0) {code[numberOfSymbols - 6] = 0; return 64;}
    if (strcmp(compareCode,"---...") == 0) {code[numberOfSymbols - 6] = 0; return 58;}
    if (strcmp(compareCode,"-....-") == 0) {code[numberOfSymbols - 6] = 0; return 45;}
    if (strcmp(compareCode,"-.--.-") == 0) {code[numberOfSymbols - 6] = 0; return 41;}
    if (strcmp(compareCode,"...-.-") == 0) {code[numberOfSymbols - 6] = 0; return 62;}
  }
  
  if (numberOfSymbols >= 5)
  {
    char* compareCode = &code[numberOfSymbols - 5];
    if (strcmp(compareCode,".-.-.") == 0) {code[numberOfSymbols - 5] = 0; return 60;}
    if (strcmp(compareCode,"-..-.") == 0) {code[numberOfSymbols - 5] = 0; return 47;}
    if (strcmp(compareCode,".----") == 0) {code[numberOfSymbols - 5] = 0; return 49;}
    if (strcmp(compareCode,"..---") == 0) {code[numberOfSymbols - 5] = 0; return 50;}
    if (strcmp(compareCode,"...--") == 0) {code[numberOfSymbols - 5] = 0; return 51;}
    if (strcmp(compareCode,"....-") == 0) {code[numberOfSymbols - 5] = 0; return 52;}
    if (strcmp(compareCode,".....") == 0) {code[numberOfSymbols - 5] = 0; return 53;}
    if (strcmp(compareCode,"-....") == 0) {code[numberOfSymbols - 5] = 0; return 54;}
    if (strcmp(compareCode,"--...") == 0) {code[numberOfSymbols - 5] = 0; return 55;}
    if (strcmp(compareCode,"---..") == 0) {code[numberOfSymbols - 5] = 0; return 56;}
    if (strcmp(compareCode,"----.") == 0) {code[numberOfSymbols - 5] = 0; return 57;}
    if (strcmp(compareCode,"-----") == 0) {code[numberOfSymbols - 5] = 0; return 48;}
    if (strcmp(compareCode,"-.--.") == 0) {code[numberOfSymbols - 5] = 0; return 40;}
    if (strcmp(compareCode,".-...") == 0) {code[numberOfSymbols - 5] = 0; return 95;}
    if (strcmp(compareCode,".--.-") == 0) {code[numberOfSymbols - 5] = 0; return 6;}
    if (strcmp(compareCode,"...-.") == 0) {code[numberOfSymbols - 5] = 0; return 126;}
  }
  
  if (numberOfSymbols >= 4)
  {
    char* compareCode = &code[numberOfSymbols - 4];
    if (strcmp(compareCode,"-...") == 0) {code[numberOfSymbols - 4] = 0; return 66;}
    if (strcmp(compareCode,"-.-.") == 0) {code[numberOfSymbols - 4] = 0; return 67;}
    if (strcmp(compareCode,"..-.") == 0) {code[numberOfSymbols - 4] = 0; return 70;}
    if (strcmp(compareCode,"....") == 0) {code[numberOfSymbols - 4] = 0; return 72;}
    if (strcmp(compareCode,".---") == 0) {code[numberOfSymbols - 4] = 0; return 74;}
    if (strcmp(compareCode,".-..") == 0) {code[numberOfSymbols - 4] = 0; return 76;}
    if (strcmp(compareCode,".--.") == 0) {code[numberOfSymbols - 4] = 0; return 80;}
    if (strcmp(compareCode,"--.-") == 0) {code[numberOfSymbols - 4] = 0; return 81;}
    if (strcmp(compareCode,"...-") == 0) {code[numberOfSymbols - 4] = 0; return 86;}
    if (strcmp(compareCode,"-..-") == 0) {code[numberOfSymbols - 4] = 0; return 88;}
    if (strcmp(compareCode,"-.--") == 0) {code[numberOfSymbols - 4] = 0; return 89;}
    if (strcmp(compareCode,"--..") == 0) {code[numberOfSymbols - 4] = 0; return 90;}
    if (strcmp(compareCode,".-.-") == 0) {code[numberOfSymbols - 4] = 0; return 3;}
    if (strcmp(compareCode,"---.") == 0) {code[numberOfSymbols - 4] = 0; return 4;}
  }
    
  if (numberOfSymbols >= 3)
  {
    char* compareCode = &code[numberOfSymbols - 3];
    if (strcmp(compareCode,"-..") == 0) {code[numberOfSymbols - 3] = 0; return 68;}
    if (strcmp(compareCode,"--.") == 0) {code[numberOfSymbols - 3] = 0; return 71;}
    if (strcmp(compareCode,"-.-") == 0) {code[numberOfSymbols - 3] = 0; return 75;}
    if (strcmp(compareCode,"---") == 0) {code[numberOfSymbols - 3] = 0; return 79;}
    if (strcmp(compareCode,".-.") == 0) {code[numberOfSymbols - 3] = 0; return 82;}
    if (strcmp(compareCode,"...") == 0) {code[numberOfSymbols - 3] = 0; return 83;}
    if (strcmp(compareCode,"..-") == 0) {code[numberOfSymbols - 3] = 0; return 85;}
    if (strcmp(compareCode,".--") == 0) {code[numberOfSymbols - 3] = 0; return 87;}
  }
  
  if (numberOfSymbols >= 2)
  {
    char* compareCode = &code[numberOfSymbols - 2];
    if (strcmp(compareCode,".-") == 0) {code[numberOfSymbols - 2] = 0; return 65;}
    if (strcmp(compareCode,"..") == 0) {code[numberOfSymbols - 2] = 0; return 73;}
    if (strcmp(compareCode,"--") == 0) {code[numberOfSymbols - 2] = 0; return 77;}
    if (strcmp(compareCode,"-.") == 0) {code[numberOfSymbols - 2] = 0; return 78;}
  }
  
  if (numberOfSymbols >= 1)
  {
    char* compareCode = &code[numberOfSymbols - 1];
    if (strcmp(compareCode,"-") == 0) {code[numberOfSymbols - 1] = 0; return 84;}
    if (strcmp(compareCode,".") == 0) {code[numberOfSymbols - 1] = 0; return 69;}
  }

  return 0;
}

void CWDecoder::Decode_Morse(float magnitude, int magnitudelimit_low)
{
  // static so value is retained between executions
  static unsigned long starttimehigh;
  static unsigned long highduration;
  static unsigned long lasthighduration;
  static unsigned long hightimesavg;
  static unsigned long startttimelow;
  static unsigned long lowduration;
  
  static float magnitudelimit = 30;
  static char realstatebefore = LOW;
  static char filteredstate = LOW;
  static char filteredstatebefore = LOW;
  static unsigned long laststarttime = 0;
  
  static char code[MORSE_SYMBOL_BUFFER_SIZE + 1] = {0}; 
  static uint8_t stop = LOW;
  static uint8_t wpm;

  char realstate = LOW;

  unsigned long currentTime = millis();
  
  //magnitudelimit auto Increase
  if (magnitude > magnitudelimit_low)
  {
    magnitudelimit = constrain(
                                 (magnitudelimit + ((magnitude - magnitudelimit) / (float)MAGNATUDE_MOVING_AVERAGE_SIZE))  /// moving average filter (Single pole low pass filter)
                               , (float)magnitudelimit_low
                               , 32000.0); 
  }
  
  realstate = (magnitude > magnitudelimit * (float)MAGNATUDE_HIGH_THRESHOLD) ? HIGH : LOW;

  if (realstate != realstatebefore)
    laststarttime = currentTime;

  if (realstate != filteredstate && (currentTime - laststarttime) > NOISE_BLANKER_MS)
  {
    filteredstate = realstate;
  }

  if (filteredstate != filteredstatebefore)
  {
    if (filteredstate == HIGH)
    {
      starttimehigh = currentTime;
      lowduration = (currentTime - startttimelow);
    }
    
    if (filteredstate == LOW)
    {
      startttimelow = currentTime;
      highduration = (currentTime - starttimehigh);

      if (highduration < MAX_HIGH_DURATION_MS)
      {
        if (highduration < (2 * hightimesavg) || hightimesavg == 0)
        {
          hightimesavg = (highduration + (hightimesavg * ((long)HIGH_TIME_MOVING_AVERAGE_SIZE - 1L))) / (long)HIGH_TIME_MOVING_AVERAGE_SIZE;     // now we know avg dit time ( rolling avg)
        }
        
        if (highduration > (5 * hightimesavg) )
        {
          hightimesavg = highduration + hightimesavg;     // if speed decrease fast ..
        }
      }
    }
  }

  ///////////////////////////////////////////////////////////////
  // now we will check which kind of baud we have - dit or dah //
  // and what kind of pause we do have 1 - 3 or 7 pause        //
  // we think that hightimeavg = 1 bit                         //
  ///////////////////////////////////////////////////////////////
  
  if (filteredstate != filteredstatebefore)
  {
    stop = LOW;
    if (filteredstate == LOW)
    {
      if (highduration < (hightimesavg*2) && highduration > (hightimesavg * DIT_MINIMUM_SIZE))  // DIT_MINIMUM_SIZE filter out false dits
      {
        strcat(code,".");
      }
      else if (highduration > (hightimesavg*2) && highduration < (hightimesavg * DAH_MAXIMUM_SIZE ))
      {
        strcat(code,"-");
        wpm = constrain(
                          (uint8_t)(((long)wpm * ((long)WORDS_PER_MIN_MOVING_AVERAGE_SIZE - 1L) + (1200L * 3L / highduration)) / (long)WORDS_PER_MIN_MOVING_AVERAGE_SIZE)  // the most precise we can do ;o)
                        , 1
                        , 255);  
      }
      else
      {
        // DAH is too long, so things are ambiguous.  try to decode what we have then start over
        docode(code);
        code[0] = '\0';
      }
    }
    
    if (strlen(code) >= MORSE_SYMBOL_BUFFER_SIZE)
    {
      // protect against too many symbols overflowing array before a character space
      // shift symbols to make room for the next one that arrives
      for (uint8_t x = 0; x < MORSE_SYMBOL_BUFFER_SIZE; x++)
      {
        code[x] = code[x+1];
      }
    }
 
    if (filteredstate == HIGH)
    {
      float lacktime = DEFAULT_LACK_TIME;
      if (wpm > 35) lacktime = WPM_35_LACK_TIME;        // when high speeds we have to have a little more pause before new letter or new word 
      else if (wpm > 30) lacktime = WPM_30_LACK_TIME;
      else if (wpm > 25) lacktime = WPM_25_LACK_TIME; 
      
      if (lowduration > (hightimesavg * (2 * lacktime)) && lowduration < hightimesavg * (5 * lacktime))  // letter space
      {
        docode(code);
        code[0] = '\0';
      }
      if (lowduration >= hightimesavg * (5 * lacktime))
      { // word space
        docode(code);
        code[0] = '\0';
        SaveDisplayCharacter(32);
      }
    }
  }

  if ((currentTime - startttimelow) > (highduration * 6) && stop == LOW)
  {
    docode(code);
    code[0] = '\0';
    stop = HIGH;
  }
 
  realstatebefore = realstate;
  lasthighduration = highduration;
  filteredstatebefore = filteredstate;
}
