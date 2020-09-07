

#ifndef fftfunctions_h
#define fftfunctions_h

double CalculateCoeff(uint8_t freqIndex, int sampleSize);
double GetMagnatude(double coefficient, int sampleSize, int *samples);
int GrepADC(int readSampleCount, int *readArray);

#endif
