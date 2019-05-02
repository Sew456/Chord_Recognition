//
// Created by wenta on 2019/4/9.
//

#include "mic_main.h"
#include <iostream>
#include <string>
#include "kiss_fft/kiss_fft.h"
#include <complex>
#include "functions.h"
#include "CTT.h"

using namespace std;

// declare JNI Function
extern "C" {
JNIEXPORT jint JNICALL
    Java_ece420_chord_1recognition_MIC_getChord(JNIEnv *env, jclass);
}

#define F_S 48000
#define VOICED_THRESHOLD 1000000000000  // Find your own threshold

int lastChordDetected = -1;
int lastRChordDetected = -1;
int lastLChordDetected = -1;

int initialpreviouschord = -1;

void ece420ProcessFrame(sample_buf *dataBuf) {
    // Variables Declaration
    int FFT_SIZE = FRAME_SIZE*zeropadmult;
    float fftmagdata[fftsize];
    float fref = 16.35160;
    float PCP[12];
    float Es = 0;   // signal energy


    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);


    // Data is encoded in signed PCM-16, little-endian, mono
    float bufferIn[FRAME_SIZE] = {0};
    float bufferIn_filtered[FRAME_SIZE] = {0};

    for (int i = 0; i < FRAME_SIZE; i++) {
        int16_t val = ((uint16_t) dataBuf->buf_[2 * i]) | (((uint16_t) dataBuf->buf_[2 * i + 1]) << 8);
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
    genPCP(fftmagdata,F_S,fref,FFT_SIZE,PCP);

    // normalize PCP
    normalizeVector(PCP, 12);




/*
    for(int i = 0; i < 12; i++){
        LOGD("PCP[ %d ] = %f \n", i, PCP[i]);
    }
*/
    // find closest matching chord using nearest neighbors
    int nextidx = nearestNeighbors(PCP,12);

    if(nextidx != -1 && nextidx == initialpreviouschord)
        lastChordDetected = nextidx;

    initialpreviouschord = nextidx;


    gettimeofday(&end, NULL);
    LOGD("Time delay: %ld us",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

}

void ece420ProcessFrameMultipleHands(sample_buf *dataBuf) {
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
        int16_t val = ((uint16_t) dataBuf->buf_[2 * i]) | (((uint16_t) dataBuf->buf_[2 * i + 1]) << 8);
        bufferIn[i] = (float) val;
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

    // generate FFT for frame
    generateFFT(bufferIn_filtered,fftmagdata);

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
    for(int i = 0; i < 12; i++){
        LOGD("RPCP[ %d ] = %f \nLPCP[ %d ] = %f \n", i, RPCP[i], i, LPCP[i]);
    }

    // find closest matching chord using nearest neighbors
    int nextleftidx = nearestNeighbors(LPCP,12);
    int nextrightidx = nearestNeighbors(RPCP,12);


    //set the current detected chord
    if(!isRChord)
        lastRChordDetected = -1;
    else if(nextrightidx != -1)
        lastRChordDetected = nextrightidx;
    if(!isLChord)
        lastLChordDetected = -1;
    else if(nextleftidx != -1)
        lastLChordDetected = nextleftidx;



    gettimeofday(&end, NULL);
    LOGD("Time delay: %ld us",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

}




// http://stackoverflow.com/questions/34168791/ndk-work-with-floatbuffer-as-parameter
JNIEXPORT jint JNICALL
    Java_ece420_chord_1recognition_MIC_getChord(JNIEnv *env, jclass) {
        return lastChordDetected + 1;
}


