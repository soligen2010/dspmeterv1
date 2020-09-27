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

To use this, be sure to review Configuration.h and make any necessary configuration changes for your rig. 

Dennis WC8C

## Change Goals
I began with a several goals:
- Get the the CW Decode to work for me, and possibly improve it
- Free memory so I can add some future new functionality I have in mind
- Integrate my Power and SWR meter changes.  I originally developed this before Dr. Lee released his. I wish I would have uploaded it sooner to save him some effort.
- Re-organize the code to make it easier to follow and modify/extend

In the process of pursuing these, I modified quite a lot of miscellaneous things.

## V2 Changes
### CW Decode
I re-arranged the decode checks so that the longer dit-dah sequences are checked first, and changed to compare to match against the end of the decoded dit-dah string.
What this does is sometimes allow a match to be found if the previous character spacing is missed.  So, for example, if two letters are run together
instead of finding no match, there will be a match on the second letter. After this, the remaining dit-dah sequences are kept to attempt to get the previous character from the sequence.
Decoding when a character space is missed is a highly ambiguous situation, and this is an attempt to occasionally still get a correct decode.

The dit-dah buffer was exposed to a potential buffer over-run situation.  I protected against this and was able to reduce the size of the buffer to save some RAM.

Changed saved timer variables so that timer rollovers are handled properly. Code streamlining and performance improvements.

The constants that control the algorithm are now in CWDecoder.h so experimentation with these is much easier.

Added filter to ignore long high states so moving averages are not skewed too badly.  Also a DAH that is too long triggers an attempt to decode and resets the DIT_DAH collection.

### Freeing Memory
I removed use of global variables wherever possible,  I changed use of byte arrays for constants to strings using the F macro to store them in PROGMEM.
I did not change the dit-dah constants to use the F macro because there are a lot of compares and I didn't want to slow it down too much.  Reduced the memory needed for FFT (see the FFT section for more details).

### S-Meter Changes

S-Meter operation has been improved in several ways.
#### S-Meter Accuracy
I added a divisor (changeable with a #define that can be applied to the SMeter and FFT data.  This allows 2 things to happen
- The SMeter values can be calibrated (it was off for my board)
- You can get more low-end resolution in the FFT by using an op amp to boost the VOL-HIGH signal.  For me this reduced the noise floor in the spectrum analyzer.  The idea is that improving the FFT also improves the CW Decode (I am uncertain how much this helped).  See the S_Meter Boost Circuit section below.

#### Faster S-Meter

The SMeter update rate is faster, although it slows back down when the FFT spectrum is displayed.

#### High Resolution S-Meter

The original code sent the Nextion S-Meter values as an integer from 0 to 8.  If you enable HIGH_RESOLUTION_SMETER in Configuration.h, the S-Meter values sent to the Nextion will be 0 to 80 (only the Nextion output changes, the I2C response is still 0 - 8).  This increased resolution works best with the S-Meter boost circuit, but still has benefit without the boost.

To use this feature, the NExtion UI needs to be updated to expect 0 to 80 instead of 0 to 8. To update a Nextion UI (assuming it shared the same original code base as the WC8C UI) make the changes below.  Commented lines are the old, uncommented are new.

On screen px in timer tmMain:
````
 //Draw Smeter
  if(pm.cp.val!=-1)
  {
    jSMeter.val=pm.cp.val
    //jSMeter.val=pm.cp.val*10
    pm.cp.val=-1
````

### FFT Changes
This is a large change.  The goal was to reduce memory usage. A substantial memory savings was realized.

I removed the existing FFT algorithm entirely and instead changed it to use the Goertzel algorithm that the CW Decode process uses.  This calculates each of
the 64 required values independently, so uses more CPU, but allowed the Imaginary sample array to be removed, and the real sample array to be changed from a float to an int
data-type.  This reduced the sample array memory required by 75%.

To mitigate the additional processing time, I changed the software serial functions to use a modified version of the AltSoftSerial library.  This library uses 
interrupts to send the data instead if in-line timing loops.  What this did was allow me interleave the FFT calculations while the data is being sent.

AltSoftSerial was modified to reduce the RX buffer (since it is used for TX only) and removed definitions for other platforms so the .h files could be combined into one.  Also added a method called TxBufferIsEmpty so that we can abort the FFT sampling if all the data hasn't been sent yet. This is to help keep the SW Serial interrupts from disrupting the sample timing.

I also coded things so that it is easy to change between using the hardware vs. software serial, which should improve the interleaving of FFT calculations and sending the data even more. (See the Serial sections below.)

### FFT Frequency Correction
I aligned the frequency of signal peaks on the main screen vs. the DSP/CW Decode screen vs. the display in FLDigi (there was a mis-match).  They all now agree on the frequency of the peak.  As a result of this change, each line in the FFT is now
50 Hz wide (was previously ~47) and the FFT frequency range now is 50 - 3200 Hz (previously stopped at 3000 Hz).

Changes to the Nextion UI are needed ti see accurate numbers.  The WC8C UI files have this change.  To update a different Nextion UI (assuming it shared the same original code base as the WC8C UI) make the changes below.  Commented lines are the old, uncommented are new.

In screen px in Timer tm1
```
  //nTemp0.val=nTemp0.val*47
  //nTemp0.val=nTemp0.val+30
  nTemp0.val=nTemp0.val*50
  nTemp0.val=nTemp0.val+50
```
In screen pdsp in Timer tm1
```
  //sys1=nDecodeFreq.val/48
  sys1=nDecodeFreq.val/50
```
### Code Reorganization and Streamlining
I was changing so much that I went ahead and did a major re-structuring and stream-lining of the code.  This is too much to fully detail in the summary.  You will see many more files and some 
functionality is now encapsulated in classes.  I personally find this type of code structure and organization easier to work with.

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

I also included my modified Nextion UI which displays the power and SWR. This is for 3.2" display only.

- The power and SWR appears on the main screen when in transmit mode.
- SWR will flash Yellow when between 2 and 3.
- SWR will flash Red when >= 3.
- Touching the Power/SWR area goes to a screen that also has a graphical Power & SWR meters.

### Hardware Serial
In i2cmeter2.h, commenting out #define USE_SW_SERIAL will cause the hardware serial to be used instead of the software serial, which frees some resources in
the sketch.  To do this you have to wire pin 0 to the Nextion instead of pin 9.  I installed a switch to select one or the other - hardware serial for normal operations and software serial for debugging. 

As a side note, I also installed a switch to disconnect pin 1 from the Raduino while flashing.

### Faster Serial Speed
Normally, the Nextion runs at 9600 baud.  I run at 57600 baud.  To do this you need to change the Raduino and Nextion firmware in addition to the Configuration.h file in i2cmeter2.  

In the CEC firmware, the speed is in the LCDNextion_Init() in the ubitx_lcd_nextion file

For the Nextion, in the Nextion editor on the pboot screen in the Event Preinitialization 
put baud=\<rate\> on the first line, for example:
```
baud=57600
```
The increased speed improves the UI responsiveness a little.  More significantly for me, when running at 9600 baud what I assume is noise  interferes with my CAT control to WSJT-X.  At 57600 the interference seems to be cured.

### Miscellaneous
Frequency changes seemed to lag a bit when the spectrum was on the main screen.  Changed so that any communication incoming from the uBix will cancel other communication processes so that these commands get forwarded quicker.  This helps the frequency changes to be more responsive when the spectrum is on the display, but the spectrum display pauses while changing frequency.

Performance improvements and code streamlining in many places.

Encapsulated many things in classes and re-factored some of the functional dependencies.

In may cases there needs to be at least a 50ms delay after certain commands to allow the Nextion to process the previous command.  I changed the way this is done and enforced the delay for all commands (except sending FFT which didn't seem to need it).  I also reduced the delay timer to 75ms. which seems adequate (it formerly was 159, but not implemented everywhere).  If you have odd Nextion update issues (I had some when I tried running at 50ms) you can try increasing #define LAST_TIME_INTERVAL in NextionProtocol.h.

## S-Meter Boost Circuit
Included is a schematic of the op amp circuit I used to increase the level of the VOL-HIGH input.  I noticed that lower level signals seemed to have very low resolution in the ADC.  By amplifying the signal, there is more resolution at the low end, but it maxes out the high end sooner.  I also added a Zener diode to protect the ADC input.  The amplification is compensated by adjusting the #define SMETER_GAIN in i2cmeter2.h (raise the number to lower the displayed value.)  To calibrate, feed the radio an S9 level signal and adjust the value until the meter just reaches the S9 marking. Note that without the high resolution S-Meter option enabled, the display will either be above or below the S9 marking - your choice on which to calibrate to.

The SMETER_GAIN setting I used for my uBitx V4 board with the op-amp circuit is 1.6.  Since I have made some mods to my uBitx, your calibration may vary.

With the op amp circuit I used, the ADC maxes out at about S9 + 10db.  However, I run the AGC from kit-projects.com, and with the AGC turned on, the ADC doesn't max out.  Personally, I think the S-Meter should be calibrated with the AGC off, but you can do it either way at your preference.

## Nextion Display
I also included my customized Nextion firmware (3.2" display)
* Tweaks to go with the changes above.
* Added Power and SWR meter screen (graphical)
* Added Power and SWR (numeric) to main screen when transmitting - SWR flashes yellow/red when too high
* Streamlined MEM to VFO screen
	* Frequencies are loaded when the screen opens
	* Selecting a frequency immediately returns to main screen
* I think there may be other tweaks I did some time ago - I don't recall the specifics.

## Known Issues
Some of the I2C responses seem like they have issues (comments are in the code).  However, I don't think the I2C interface gets used if a Nextion display is used.  I tried it retain the original responses even if there seemed to be an issue.

## License
I follow the same license terms as Dr. Lee. I do not claim any license for this code.
You may use it in any way. I just hope this will be used for amateur radio.
The other person's source code (CW Morse code) follows the original author's license.

The AltSoftSerial Code follows the original author's license

Dennis Cabell WC8C