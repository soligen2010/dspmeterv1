/*
FFTFunctions for Nextion LCD and Control MCU
This code is for FFT and CW Decode. 
KD8CEC, Ian Lee
-----------------------------------------------------------------------
Aug 2020: Refactored and modifed by WC8C, Dennis Cabell
License: Same as origial code.
**********************************************************************/

///////////////////////////////////////////////////////////////////////////
// Read more here http://en.wikipedia.org/wiki/Goertzel_algorithm        //
// if you want to know about FFT the http://www.dspguide.com/pdfbook.htm //
///////////////////////////////////////////////////////////////////////////

#include <arduino.h>
#include "Configuration.h"
#include "i2cmeter2.h"

double CalculateCoeff(int freqIndex, int sampleSize)
{
  // calculates the coeeficient for the Goertzel_algorithm
  // freqIndex is which frequency bin to calculate.  This is based on the largest frequence (3000 Hz) and the FFT size

  int targetFrequency = (freqIndex * (float)MAX_FFT_FREQUENCY + (FFTSIZE/2)) / FFTSIZE;  // bin size is 3k / fft size.  The (FFTSIZE/2) centers the frequency in the bin
  double k = (int) (0.5 + (((long)sampleSize * targetFrequency) / (double)SAMPLE_FREQUENCY));
  double omega = (2.0 * PI * k) / (double)sampleSize;
  return 2.0 * cos(omega);
}

double GetMagnatude(double coefficient, int sampleSize, int *samples)
{
    //Implement Goertzel_algorithm
    //https://en.wikipedia.org/wiki/Goertzel_algorithm

    /*
    ω = 2 * π * Kterm / Nterms;
    cr = cos(ω);
    ci = sin(ω);
    coeff = 2 * cr;
    
    sprev = 0;
    sprev2 = 0;
    for each index n in range 0 to Nterms-1
      s = x[n] + coeff * sprev - sprev2;
      sprev2 = sprev;
      sprev = s;
    end
    
    power = sprev2 * sprev2 + sprev * sprev - coeff * sprev * sprev2;
     */
    double Q1 = 0;
    double Q2 = 0;
    
    for (int index = 0; index < sampleSize; index++)
    {
      double Q0;
      Q0 = coefficient * Q1 - Q2 + (double)samples[index];
      Q2 = Q1;
      Q1 = Q0;
    }
    double magnitudeSquared = (Q1*Q1)+(Q2*Q2)-Q1*Q2*coefficient;  // we do only need the real part //
    return sqrt(magnitudeSquared)  / SMETER_GAIN;
}
