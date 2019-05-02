//
// Created by wenta on 2019/4/9.
//

#include "file_main.h"
#include <iostream>
#include <string>
#include "kiss_fft/kiss_fft.h"
#include <complex>
#include "functions.h"
#include "CTT.h"
#include <jni.h>

using namespace std;

// declare JNI Function
extern "C" {
JNIEXPORT jint JNICALL
    Java_ece420_chord_1recognition_FILE_getChordFile(JNIEnv *env, jclass,jdoubleArray buffer, jobject bufferPtr);
}

#define F_S 44100
#define VOICED_THRESHOLD 1000000000000  // Find your own threshold

int lastChordDetected_2 = -1;
int lastLChordDetected_2 = -1;
int lastRChordDetected_2 = -1;

int initialpreviousRchord = -1;
int initialpreviousLchord = -1;


void ece420ProcessFrame_2(double *dataBuf) {
    // Variables Declaration
    int FFT_SIZE = FRAME_SIZE*zeropadmult;
    float fftmagdata[fftsize];
    float fref = 16.35160;
    float RPCP[12], LPCP[12];
    bool isLChord = false;
    bool isRChord = false;
    float Es = 0;   // signal energy


    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);


    // Data is encoded in signed PCM-16, little-endian, mono
    float bufferIn[FRAME_SIZE] = {0};
    float bufferIn_filtered[FRAME_SIZE] = {0};

    for (int i = 0; i < FRAME_SIZE; i++) {
        //int16_t val = ((uint16_t) dataBuf[i]) | (((uint16_t) dataBuf[i + 1]) << 8);
        double val = dataBuf[i];
        bufferIn[i] = (float) val;
       // Es = Es + pow(abs(bufferIn[i]),2);
       Es += (bufferIn[i] * bufferIn[i]) / FRAME_SIZE;
    }
 //   LOGD("ES = %f", Es);
/*
    if (Es < VOICED_THRESHOLD) {
        return;
    }
*/
    // generate low-pass filter and filter incoming frame

    for (int sampleIdx = 0; sampleIdx < FRAME_SIZE; sampleIdx++) {
        // Grab one sample from bufferIn[]
        int16_t sample = bufferIn[sampleIdx];
        // Call your filFilter funcion
        int16_t output = lowpassIIRfilter(sample);
        // Grab result and put into bufferOut[]
        bufferIn_filtered[sampleIdx] = output;
    }

    //Generate zeropad array
    float bufferzeropad[fftsize];

    zeropad(bufferIn_filtered, bufferzeropad);

    // generate FFT for frame
    generateFFT(bufferzeropad,fftmagdata);

    if(!chordThreshold(fftmagdata, FFT_SIZE))       //return if not within the threshold
        return;

    // generate PCP
    generateDualPCP(fftmagdata, F_S, fref, FFT_SIZE, LPCP, RPCP);

    //CHECKS the values of the PCP to see if there are above a certain threshold

    // normalize PCP
    normalizeVector(RPCP, 12);
    normalizeVector(LPCP, 12);


    for(int i = 0; i < 12; i++){
        LOGD("RPCP[ %d ] = %f \nLPCP[ %d ] = %f \n", i, RPCP[i], i, LPCP[i]);
    }

    // find closest matching chord using nearest neighbors
    int nextleftidx = nearestNeighbors(LPCP,12);
    int nextrightidx = nearestNeighbors(RPCP,12);


    //set the current detected chord
    if(!isRChord)
        lastRChordDetected_2 = -1;
    else if(nextrightidx != -1 && initialpreviousRchord == nextrightidx)
        lastRChordDetected_2 = nextrightidx;
    if(!isLChord)
        lastLChordDetected_2 = -1;
    else if(nextleftidx != -1 && initialpreviousLchord == nextleftidx)
        lastLChordDetected_2 = nextleftidx;

    initialpreviousRchord = nextrightidx;
    initialpreviousLchord = nextleftidx;


    gettimeofday(&end, NULL);
    LOGD("Time delay: %ld us",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

}

void ece420ProcessFrameMultipleHands(double *dataBuf) {
    // Variables Declaration
    int FFT_SIZE = FRAME_SIZE*zeropadmult;
    float fftmagdata[fftsize];
    float fref = 16.35160;
    float RPCP[12], LPCP[12];
    bool isLChord = false;
    bool isRChord = false;


    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);


    // Data is encoded in signed PCM-16, little-endian, mono
    float bufferIn[FRAME_SIZE] = {0};
    float bufferIn_filtered[FRAME_SIZE] = {0};

    for (int i = 0; i < FRAME_SIZE; i++) {
        //int16_t val = ((uint16_t) dataBuf[i]) | (((uint16_t) dataBuf[i + 1]) << 8);
        double val = dataBuf[i];
        bufferIn[i] = (float) val;
        // Es = Es + pow(abs(bufferIn[i]),2);
    }

    // generate low-pass filter and filter incoming frame

    for (int sampleIdx = 0; sampleIdx < FRAME_SIZE; sampleIdx++) {
        // Grab one sample from bufferIn[]
        int16_t sample = bufferIn[sampleIdx];
        // Call your filFilter funcion
        int16_t output = lowpassIIRfilter(sample);
        // Grab result and put into bufferOut[]
        bufferIn_filtered[sampleIdx] = output;
    }

    float bufferzeropad[fftsize];

    zeropad(bufferIn_filtered, bufferzeropad);
    // generate FFT for frame
    generateFFT(bufferzeropad,fftmagdata);

    if(!chordThreshold(fftmagdata, FFT_SIZE))       //return if not within the threshold
        return;

    // generate PCP
    generateDualPCP(fftmagdata, F_S, fref, FFT_SIZE, LPCP, RPCP);

    //CHECKS the values of the PCP to see if there are above a certain threshold
    isLChord = checkLPCPthreshold(LPCP);
    isRChord = checkRPCPthreshold(RPCP);

    // normalize PCP
    normalizeVector(RPCP, 12);
    normalizeVector(LPCP, 12);

    // DEBUGGING statements
/*
    for(int i = 0; i < 12; i++){
        LOGD("RPCP[ %d ] = %f \nLPCP[ %d ] = %f \n", i, RPCP[i], i, LPCP[i]);
    }
*/
    // find closest matching chord using nearest neighbors
    int nextleftidx = nearestNeighbors(LPCP,12);
    int nextrightidx = nearestNeighbors(RPCP,12);


    //set the current detected chord
    if(nextrightidx != -1 && isRChord)
        lastRChordDetected_2 = nextrightidx;

    else if(nextleftidx != -1 && isLChord && nextleftidx == initialpreviousLchord)
        lastLChordDetected_2= nextleftidx;

    initialpreviousRchord = nextrightidx;
    initialpreviousLchord = nextleftidx;



    gettimeofday(&end, NULL);
    LOGD("Time delay: %ld us",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
}

// http://stackoverflow.com/questions/34168791/ndk-work-with-floatbuffer-as-parameter
JNIEXPORT jint JNICALL
    Java_ece420_chord_1recognition_FILE_getChordFile(JNIEnv *env, jclass,jdoubleArray buffer, jobject bufferPtr) {
    int size = 0;
    size = env->GetArrayLength(buffer);
    double dataBuffer[8192] = {0};
    jdouble *body = env->GetDoubleArrayElements(buffer,0);
    for (int i = 0;i < size;i++) {
        dataBuffer[i] = (double) body[i];
    }
    env->ReleaseDoubleArrayElements(buffer,body,0);
    ece420ProcessFrameMultipleHands(dataBuffer);

    jint *currentChords = (jint *) env->GetDirectBufferAddress(bufferPtr);
    currentChords[0] = lastRChordDetected_2 + 1;
    currentChords[1] = lastLChordDetected_2 + 1;
    return lastChordDetected_2 + 1;
}