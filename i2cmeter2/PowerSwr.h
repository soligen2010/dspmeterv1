

#ifndef PowerSwr_h
#define PowerSwr_h

#include "Configuration.h"

#ifdef ENABLE_POWER_SWR_METER
class PowerSwr
{
public:
void PowerSwrCalculation(unsigned long L_vfoCurr);

float power = 0;
float swr = 1;

int powerIn;
int revPowerIn;

protected:
  
};
#endif
#endif
