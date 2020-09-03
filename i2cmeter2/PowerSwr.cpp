
#include "Configuration.h"
#include "i2cmeter2.h"
#include "PowerSwr.h"

#ifdef ENABLE_POWER_SWR_METER

void PowerSwr::PowerSwrCalculation(unsigned long L_vfoCurr)
{
  int newPowerIn = analogRead(POWER_METER_ADC);  // read each value twice so ADC settles for more accuracy
  newPowerIn = analogRead(POWER_METER_ADC); 
  int newRevPowerIn = analogRead(REV_POWER_METER_ADC);
  newRevPowerIn = analogRead(REV_POWER_METER_ADC);

  if (newRevPowerIn > newPowerIn)    // Assume wires are backwards so swap values
  {
    newRevPowerIn ^= newPowerIn;
    newPowerIn    ^= newRevPowerIn;
    newRevPowerIn ^= newPowerIn;
  }

  noInterrupts();         // disable interrupts while saving these so that the I2C ISR cant read them while they are changing.
  powerIn = newPowerIn;
  revPowerIn = newRevPowerIn;
  interrupts();

  power = (newPowerIn  == 0) ? 0.0 : pow(CAL_REF_VOLTAGE * newPowerIn * CAL_FWD_ADJUST_MULTIPLIER(L_vfoCurr) / 1023.0 + CAL_FWD_DIODE_DROP, 2);

  if (newPowerIn==0 || newRevPowerIn == 0)
  {
    swr = 1.0;  // if no power set swr to 1
  } 
  else if (newRevPowerIn==newPowerIn)
  {
    swr = 9.9;  // set to swr 9.9 if revPower is high
  }       
  else
  {
    float revPower = (newRevPowerIn==0) ? 0.0 : pow(CAL_REF_VOLTAGE * newRevPowerIn * CAL_REV_ADJUST_MULTIPLIER(L_vfoCurr) / 1023.0 + CAL_REV_DIODE_DROP, 2);
    swr = (1.0 + sqrt(revPower/power)) / (1.0 - sqrt(revPower/power));
    swr = (swr > 9.9) ? 9.9 : swr;  // max swr sent is 9.9
  }
}

#endif
