

#ifndef CWDecoder_h
#define CWDecoder_h

#define MAX_DECODED_CHARACTERS_BUFFER_SIZE  5   // (3 - 5) do not increase beyond 5 or else characters get cut off - dont know why, its liekly an issue in the Nextion
#define MORSE_SYMBOL_BUFFER_SIZE           10   // (8 - 254) must be at least 8 becasue largest morse character needs 7, and at least one extra is needed for overflow logic not to truncate to below 7

////////////////////////////////////////////////
// These constants control the decode algorithm
////////////////////////////////////////////////
// Original algorithm values are:
// -------------------------------------
// NOISE_BLANKER_MS                  6    
//#define MAX_HIGH_DURATION_MS - not in previous version
// MAGNATUDE_MOVING_AVERAGE_SIZE     6    
// HIGH_TIME_MOVING_AVERAGE_SIZE     3   
// WORDS_PER_MIN_MOVING_AVERAGE_SIZE 2    
// DIT_MINIMUM_SIZE                  0.6  
// DAH_MAXIMUM_SIZE                  6     
// MAGNATUDE_HIGH_THRESHOLD          0.6  
// DEFAULT_LACK_TIME                 1.0  
// WPM_25_LACK_TIME                  1.0  
// WPM_30_LACK_TIME                  1.2   
// WPM_35_LACK_TIME                  1.3  
////////////////////////////////////////////////
#define NOISE_BLANKER_MS                  6     // Milliseconds.  baud less than this duration are ignored
#define MAX_HIGH_DURATION_MS              3000  // Milliseconds.  baud greater than this are excluded from the moving average calulation for code speed
#define MAGNATUDE_MOVING_AVERAGE_SIZE     6     // Raising this makes the expected magnatude of high state self adjust more slowly.  Must be > 1
#define HIGH_TIME_MOVING_AVERAGE_SIZE     3     // Raising this makes the expected dit-dah duration self adjust more slowly.  Must be > 1
#define WORDS_PER_MIN_MOVING_AVERAGE_SIZE 2     // Raising this makes the high WPM lacktime adjust more slowly.  Must be > 1
#define DIT_MINIMUM_SIZE                  0.6   // High time must be at least this percentage of the average high time to be a dit
#define DAH_MAXIMUM_SIZE                  6     // High time must be at below this percentage of the average high time to be a dah, thsi triggers a decode and resets the baud collection
#define MAGNATUDE_HIGH_THRESHOLD          0.6   // Signals above this percent of magnatude are interpreted as HIGH
#define DEFAULT_LACK_TIME                 1.0   // Adjusts how much space is expected between characters and words. < 25 WPM
#define WPM_25_LACK_TIME                  1.0   // 25 - 30 WPM
#define WPM_30_LACK_TIME                  1.2   // 30 - 35 WPM
#define WPM_35_LACK_TIME                  1.3   // over 35 WPM


class CWDecoder
{
 public:
  
  void Decode_Morse(float magnitude, int magnitudelimit_low);
  void ClearDecodedCharacters();
  char* GetDecodedCharacters();
  void docode(char *code);

protected:
  void SaveDisplayCharacter(char asciinumber);
  char getDecodeCharacter(char *code);

  char decodedCharacters[MAX_DECODED_CHARACTERS_BUFFER_SIZE + 1] = { 0 };  //Add one for 0x00 to mark the end
};
#endif
