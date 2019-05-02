//
// Created by sew on 4/12/19.
//

#ifndef CHORD_RECOGNITION_FUNCTIONS_H
#define CHORD_RECOGNITION_FUNCTIONS_H

#include <math.h>
#include <algorithm>
#include <jni.h>
#include "kiss_fft/kiss_fft.h"
#include <complex>

#define FRAME_SIZE 8192
#define zeropadmult 2
#define fftsize FRAME_SIZE*zeropadmult

int16_t lowpassIIRfilter(int16_t samplex);
bool chordThreshold(const float* fftmagdata, int length);
int genML(int F_s, int fftlen, float fref, int l);
void genPCP(const float* fftmagdata, int F_s, float fref, int fftlen, float* pcp);
int nearestNeighbors(const float* vect, int vectlen);
void normalizeVector(float* vect, int vectlen);
void generateFFT(float * bufferIn, float* fftmagdata);
void zeropad(float* vectin, float* vectout);
bool checkLPCPthreshold(const float* PCP);
bool checkRPCPthreshold(const float* PCP);

//TWO HANDED
void generateDualPCP(const float* fftmagdata, int F_s, float fref, int fftlen, float* lpcp, float* rpcp);
bool checkLeft(float freq);


#endif //CHORD_RECOGNITION_FUNCTIONS_H
