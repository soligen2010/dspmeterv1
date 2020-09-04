# i2cmeter1
Standalone Signal Analyzer (I2C Type Signal-Meter) for uBITX - Arduino Nano Version

I do not claim any license for my code.
You may use it in any way. I just hope this will be used for amateur radio.
The other person's source code (CW Morse code) follows the original author's license.

Ian KD8CEC

i2cmeter1 is the original code from Dr. Lee, kept here for reference.

# i2cmeter2
This is a large re-working of Dr. Lee's original dspmeterv1 code.  I created a new folder for this because
the changes are so extensive.

To use this version you should be familiar with using the Arduino IDE and flashing an Arduino.  The DSP Nano will likely need to disconnected from the Raduino for flashing to work.

## Change Goals
I began with a several goals:
- Get the the CW Decode to work for me, and possibly improve it
- Free memory so I can add some future new functionality I have in mind
- Integrate my Power and SWR meter changes.  I originally developed this before Dr. Lee released his. I wish I would have uploaded it sooner to save him some effort.
- Re-organize the code to make it easier to follow and modify/extend

In the process of pursuing these, I modified quite a lot of miscellaneous things.

## V2 Summary
### CW Decode
I re-arranged the decode checks so that the longer dit-dah sequences are checked first, and changed to compare to match against the end of the decoded dit-dah string.
What this does is sometimes allow a match to be found if the previous character spacing is missed.  So, for example, if two letters are run together
instead of finding no match, there will be a match on the second letter. After this, the remaining dit-dah sequences are kept to attempt to get the previous character from the sequence.
Decoding when a character space is missed is a highly ambiguous situation, and this is an attempt to occasionally still get a correct decode.

The dit-dah buffer was exposed to a potential buffer over-run situation.  I protected against this and was able to reduce the size of the buffer to save some RAM.

Changed saved timer variables so that timer rollovers are handled properly. Code streamlining and performance improvements.

The constants that control the algorithm are now in CWDecoder.h so experimentation with these is much easier.

### Freeing Memory
I removed use of global variables wherever possible,  I changed use of byte arrays for constants to strings using the F macro to store them in PROGMEM.
I did not change the dit-dah constants to use the F macro because there are a lot of compares and I didn't want to slow it down too much.  Reduced the memory needed for FFT (see the FFT section for more details).

### S-Meter Accuracy
I added a divisor (changeable with a #define that can be applied to the SMeter and FFT data.  This allows 2 things to happen
- The SMeter values can be calibrated (it was off for my board)
- You can get more low-end resolution in the FFT by using an op amp to boost the VOL-HIGH signal.  For me this reduced the noise floor in the spectrum analyzer.  The idea is that improving the FFT also improves the CW Decode (It is in-determinant if this helped).  See the S_Meter Boost Circuit section below.

### FFT Changes
This is a large change.  The goal was to reduce memory usage. A substantial memory savings was realized.

I removed the existing FFT algorithm entirely and instead changed it to use the Goertzel algorithm that the CW Decode process uses.  This calculates each of
the 64 required values independently, so used more CPU, but allowed the Imaginary sample array to be removed, and the real sample array to be changed from a float to an int
data-type.  This reduced the sample array memory required by 75%.

To mitigate the additional processing time, I changed the software serial functions to use a modified version of the AltSoftSerial library.  This library uses 
interrupts to send the data instead if in-line timing loops.  What this did was allow me interleave the FFT calculations while the data is being sent.

I also coded things so that it is easy to change between using the hardware vs. software serial, which should improve the interleaving of FFT calculations and sending the data even more.

I better aligned the frequency of signal peaks on the main screen vs. the DSP/CW Decode screen (there was a mis-match).  It is not perfect, but better.  The Peak frequencies still do not match external programs like FLDigi, but at least it is consistent.

AltSoftSerial was modified to reduce the RX buffer (since it is used for TX only) and removed definitions for other platforms so the .h files could be combined into one.  Also added a method called TxBufferIsEmpty so that we can abort the FFT sampling if all the data hasn't been sent yet. This is to help keep the SW Serial interrupts from disrupting the sample timing.

### Code Reorganization and Streamlining
I was changing so much that I went ahead and did a major re-structuring and stream-lining of the code.  This is too much to fully detail in the summary.  You will see many more files and some 
functionality is now encapsulated in classes.  I personally find this type of code structure and organization easier to deal with.

### Thread Safety
There were thread safety issues between the main loop and the I2C interrupt routines.  Most notably certain I2C commands could take over the ADC at the same time the main loop was doing a FFT.
sample, which could disrupt the FFT sample. I changed this so that
- Used volatile variables and disabled interrupts where appropriate when updating variables used in the ISR.
- The I2C function no longer reads the ADC - all the values needed are now read in the main loop, so use the most recent value.

### Power & SWR Meter
The Power and SWR meter code is not Dr. Lee's.  This version allows calibration of the power by changing values in PowerSwrCalibration.h.  Each band can be individually calibrated.

This implementation uses the same input pins as Dr. Lee, but will dynamically determine which is forward vs. reverse power and swap them in software if you wired them backwards.

I used the ND6T SWR/Power bridge I found at <http://bitxhacks.blogspot.com/2017/03/nd6ts-forward-and-reverse-power-meter.html>.  I used the circuit, but not his code.

In this implementation the max SWR value is 1:9.9.

I also included my modified Nextion UI which displays the power and SWR on the main screen when in transmit mode. This is for 3.2" display only.

### Hardware Serial
In i2cmeter2.h, commenting out #define USE_SW_SERIAL will cause the hardware serial to be used instead of the software serial, which frees some resources in
the sketch.  To do this you have to wire pin 0 to the Nextion instead of pin 9.  I installed a switch to select one or the other - hardware serial for normal operations and software serial for debugging. 

As a side note, I also installed a switch to disconnect pin 1 from the Raduino while flashing.

### Faster Serial Speed
Normally, the Nextion runs at 9600 baud.  I run at 57600 baud.  To do this you need to change the Raduino and Nextion firmware (see notes in Configuration.h) in addition to the dspmeter configuration.  

The increased speed improves the UI responsiveness a little.  More significantly for me, when running at 9600 baud what I assume is noise would interferes with my CAT control to WSJT-X.  At 57600 the interference seems to be cured.

### Miscellaneous
Frequency changes seemed to lag a bit when the spectrum was on the main screen.  Changed so that any communication incoming from the uBix will cancel other communication processes so that these commands get forwarded quicker.  This helps the frequency changes to be more responsive when the spectrum is on the display, but the spectrum display pauses while changing frequency.

Performance improvements and code streamlining in many places.

Encapsulated many things in classes and re-factored some of the functional dependencies.

In may cases there needs to be at least a 50ms delay after certain commands to allow the Nextion to process the previous command.  I changed the way this is done and enforced the delay for all commands (except sending FFT which didn't seem to need it).  I also reduced the delay timer to 75ms. which seems adequate (it formerly was 159, but not implemented everywhere).  If you have odd Nextion update issues (I had some when I tried running at 50ms) you can try increasing #define LAST_TIME_INTERVAL in NextionProtocol.h.

## S-Meter Boost Circuit
Included is a schematic of the op amp circuit I used to increase the level of the VOL-HIGH input.  I noticed that lower level signals seemed to have very low resolution in the ADC.  By amplifying the signal, there is more resolution at the low end, but it maxes out the high end sooner.  I also added a Zener diode to protect the ADC input.  The amplification is compensated by adjusting the #define SMETER_GAIN in i2cmeter2.h (raise the number to lower the displayed value.)  To calibrate, feed the radio an S9 level signal and adjust the value until the meter just passes the S9 reading.

The SMETER_GAIN setting I used for my uBitx V4 board with the op-amp circuit is 1.5.  Since I have made come mods to my uBitx, your calibration may vary.

With the op amp circuit I used, the ADC maxes out at about S9 + 10db.  However, I run an AGC, and with the AGC turned on, the ADC doesn't max out.  Ideally I think the SMeter should be calibrated without the AGC, but you can do it either way at your preference.

## License
I follow the same license terms as Dr. Lee. I do not claim any license for this code.
You may use it in any way. I just hope this will be used for amateur radio.
The other person's source code (CW Morse code) follows the original author's license.

The AltSoftSerial Code follows the original author's license

Dennis Cabell WC8C