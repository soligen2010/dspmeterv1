/* An Alternative Software Serial Library
 * http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
 * Copyright (c) 2014 PJRC.COM, LLC, Paul Stoffregen, paul@pjrc.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef AltSoftSerial_h
#define AltSoftSerial_h

#include <inttypes.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#if defined(__arm__) && defined(CORE_TEENSY)
#define ALTSS_BASE_FREQ F_BUS
#else
#define ALTSS_BASE_FREQ F_CPU
#endif

class AltSoftSerial : public Stream
{
public:
  AltSoftSerial() { }
  ~AltSoftSerial() { end(); }
  static void begin(uint32_t baud) { init((ALTSS_BASE_FREQ + baud / 2) / baud); }
  static void end();
  int peek();
  int read();
  int available();
  int availableForWrite();
  bool TxBufferIsEmpty();
#if ARDUINO >= 100
  size_t write(uint8_t byte) { writeByte(byte); return 1; }
  void flush() { flushOutput(); }
#else
  void write(uint8_t byte) { writeByte(byte); }
  void flush() { flushInput(); }
#endif
  using Print::write;
  static void flushInput();
  static void flushOutput();
  // for drop-in compatibility with NewSoftSerial, rxPin & txPin ignored
  AltSoftSerial(uint8_t rxPin, uint8_t txPin, bool inverse = false) { }
  bool listen() { return false; }
  bool isListening() { return true; }
  bool overflow() { bool r = timing_error; timing_error = false; return r; }
  static int library_version() { return 1; }
  static void enable_timer0(bool enable) { }
  static bool timing_error;
private:
  static void init(uint32_t cycles_per_bit);
  static void writeByte(uint8_t byte);
};


// Arduino Uno, Duemilanove, LilyPad, etc
//
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)

  #define ALTSS_USE_TIMER1
  #define INPUT_CAPTURE_PIN     8 // receive
  #define OUTPUT_COMPARE_A_PIN    9 // transmit
  #define OUTPUT_COMPARE_B_PIN   10 // unusable PWM

  #define CONFIG_TIMER_NOPRESCALE()  (TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS10))
  #define CONFIG_TIMER_PRESCALE_8() (TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS11))
  #define CONFIG_TIMER_PRESCALE_256() (TIMSK1 = 0, TCCR1A = 0, TCCR1B = (1<<ICNC1) | (1<<CS12))
  #define CONFIG_MATCH_NORMAL()   (TCCR1A = TCCR1A & ~((1<<COM1A1) | (1<<COM1A0)))
  #define CONFIG_MATCH_TOGGLE()   (TCCR1A = (TCCR1A & ~(1<<COM1A1)) | (1<<COM1A0))
  #define CONFIG_MATCH_CLEAR()    (TCCR1A = (TCCR1A | (1<<COM1A1)) & ~(1<<COM1A0))
  #define CONFIG_MATCH_SET()    (TCCR1A = TCCR1A | ((1<<COM1A1) | (1<<COM1A0)))
  #define CONFIG_CAPTURE_FALLING_EDGE() (TCCR1B &= ~(1<<ICES1))
  #define CONFIG_CAPTURE_RISING_EDGE()  (TCCR1B |= (1<<ICES1))
  #define ENABLE_INT_INPUT_CAPTURE()  (TIFR1 = (1<<ICF1), TIMSK1 = (1<<ICIE1))
  #define ENABLE_INT_COMPARE_A()  (TIFR1 = (1<<OCF1A), TIMSK1 |= (1<<OCIE1A))
  #define ENABLE_INT_COMPARE_B()  (TIFR1 = (1<<OCF1B), TIMSK1 |= (1<<OCIE1B))
  #define DISABLE_INT_INPUT_CAPTURE() (TIMSK1 &= ~(1<<ICIE1))
  #define DISABLE_INT_COMPARE_A() (TIMSK1 &= ~(1<<OCIE1A))
  #define DISABLE_INT_COMPARE_B() (TIMSK1 &= ~(1<<OCIE1B))
  #define GET_TIMER_COUNT()   (TCNT1)
  #define GET_INPUT_CAPTURE()   (ICR1)
  #define GET_COMPARE_A()   (OCR1A)
  #define GET_COMPARE_B()   (OCR1B)
  #define SET_COMPARE_A(val)    (OCR1A = (val))
  #define SET_COMPARE_B(val)    (OCR1B = (val))
  #define CAPTURE_INTERRUPT   TIMER1_CAPT_vect
  #define COMPARE_A_INTERRUPT   TIMER1_COMPA_vect
  #define COMPARE_B_INTERRUPT   TIMER1_COMPB_vect

// Unknown board
#else
#error "Please define your board timer and pins"
#endif

#endif
