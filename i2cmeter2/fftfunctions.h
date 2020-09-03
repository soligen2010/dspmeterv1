

#ifndef fftfunctions_h
#define fftfunctions_h

double CalculateCoeff(int freqIndex, int sampleSize);
double GetMagnatude(double coefficient, int sampleSize, int *samples);
int GrepADC(int readSampleCount, int *readArray);

#endif
