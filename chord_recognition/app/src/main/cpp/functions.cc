/* IIR Filter Coefficients
	b = {0.0011514725696540262, 0.005757362848270131, 0.011514725696540261, 0.011514725696540261, 0.005757362848270131, 0.0011514725696540262}
	a = {1.0, -3.025546952214802, 3.916551235363577, -2.645063126276531, 0.9232828442524519, -0.1323768788957676};
*/
#include "functions.h"
#include "CTT.h"
#include "android_debug.h"


#define N_TAPS 6

double myFilterb[N_TAPS] = {0.0011514725696540262, 0.005757362848270131, 0.011514725696540261, 0.011514725696540261, 0.005757362848270131, 0.0011514725696540262};
double myFiltera[N_TAPS] = {1.0, -3.025546952214802, 3.916551235363577, -2.645063126276531, 0.9232828442524519, -0.1323768788957676};

/*	IIR FILTER CALCULATION
 *
 *
*/

//TODO All Arrays need to be passed as an input not a return value;

int16_t circbufY[N_TAPS] = {0};
int16_t circbufX[N_TAPS] = {0};
int circBufIdx = 0;

int16_t lowpassIIRfilter(int16_t samplex){
	int16_t output = 0;
	circbufX[circBufIdx] = samplex;
	int bufindex = 0;
  
	for(int i = 0; i < N_TAPS; i++){
  
		if(circBufIdx - i < 0)
			bufindex = circBufIdx-i + N_TAPS;
		else
			bufindex = circBufIdx - i;

		if(i != 0)
		    output += (int16_t) (circbufX[bufindex]*myFilterb[i] - circbufY[bufindex]*myFiltera[i]);
		else
		    output += (int16_t) (circbufX[bufindex]*myFilterb[i]);
	}
	output = (int16_t) (output/myFiltera[0]);

    circbufY[circBufIdx] = output;                      //add current Y term to the circular buffer
  
      if(circBufIdx == N_TAPS-1){
          circBufIdx = 0;
      }
      else{
          circBufIdx++;
     }

    return output;
}


bool chordThreshold(const float* fftmagdata, int length){
	bool isChord = false;
    float sum = 0;
	for(int i = 0; i < length; i++ ){
		sum += fftmagdata[i]*fftmagdata[i];
	}

  //  LOGD("SUM = %f", sum);
	if(sum > 0) {
        isChord = true;

    }

	return isChord;
}

int genML(int F_s, int fftlen, float fref, int l){
	int ml = 0;
	if(l != 0) {
	    float freqnorm = F_s*l/(fftlen*fref);
        float val = 12.f * log2f(freqnorm);
        int intval = (float) roundf((float) val);
        ml = (intval % 12);            //Main chord Algorithm
    }
    else
		ml = -1;

	return ml;
	
}

//TODO add ml and change it so it is faster (pregenerate it)
void genPCP(const float* fftmagdata, int F_s, float fref, int fftlen, float* pcp){
	int N = fftlen / 2 - 1;
	int ML = 0;

	for(int l = 0; l < N; l++){
	    ML = genML(F_s, fftlen, fref, l);
	    if(ML >= 0)
		    pcp[ML] += fftmagdata[l] * fftmagdata[l];
	}

}

int nearestNeighbors(const float* vect, int vectlen){
    float score[CTT_LENGTH];
    for(int i = 0; i < CTT_LENGTH; i++){
        score[i] = 0;
        for(int p = 0; p < vectlen; p++){
            score[i] += (CTT[i][p] - vect[p])*(CTT[i][p] - vect[p]);
        }
    }

    float minval = 100000;
    int minidx = -1;

    for(int i = 0; i < CTT_LENGTH; i++){
        if(minval > score[i]){
            minval = score[i];
            minidx = i;
        }
    }

    if(minval < 1.25)
        return minidx;
    else
        return -1;

}

void normalizeVector(float* vect, int vectlen){
	float maxele = *std::max_element(vect, vect + vectlen);

	for(int i = 0; i < vectlen; i++){
		vect[i] = vect[i] / maxele;
	}

}

void zeropad(float* vectin, float* vectout){
    for(int i = 0; i < fftsize; i++){
        if (i < FRAME_SIZE){
            vectout[i] = vectin[i];
        } else{
            vectout[i] = 0;
        }
    }
}


void generateFFT( float* bufferIn, float* fftmagdata){

	kiss_fft_cfg cfg = kiss_fft_alloc( fftsize ,0 ,0,0 );
	auto cx_in = new kiss_fft_cpx[fftsize];
	auto cx_out = new kiss_fft_cpx[fftsize];
	
	//Put data into input fft array
	for(int k = 0; k < fftsize; k++){
        	cx_in[k].r = bufferIn[k];
        	cx_in[k].i = 0;
    	}	

	//Do the FFT
	kiss_fft(cfg, cx_in, cx_out);

	//Cx_in is unneeded
	kiss_fft_free(cx_in);

	for(int k = 0; k < fftsize; k++) {
		fftmagdata[k] = sqrt(cx_out[k].r * cx_out[k].r + cx_out[k].i * cx_out[k].i) / fftsize ;
	}

	kiss_fft_free(cx_out);


}

//mulitple hands functions

bool checkLPCPthreshold(const float* PCP){
    bool isChord = false;
    float sum = 0;
    for(int i = 0; i < 12; i++ ){
        sum += PCP[i]*PCP[i];
    }

//    LOGD("SUM = %f", sum);
    if(sum > 10000000000000) {
        isChord = true;
    }

    return isChord;
}

bool checkRPCPthreshold(const float* PCP){
    bool isChord = false;
    float sum = 0;
    for(int i = 0; i < 12; i++ ){
        sum += PCP[i]*PCP[i];
    }

//    LOGD("SUM = %f", sum);
    if(sum > 700000000000000) {
        isChord = true;
    }

    return isChord;
}

bool checkLeft(float freq){

    if (freq < 261.6256)        //Check if the the current value is within the left hand or right hand
        return true;
    else
        return false;
}

void generateDualPCP(const float* fftmagdata, int F_s, float fref, int fftlen, float* lpcp, float* rpcp){
    int N = fftlen / 2 - 1;
    int ML = 0;
    float freq = 0;
    for(int l = 0; l < N; l++){
        freq = F_s * l / fftlen;
        ML = genML(F_s, fftlen, fref, l);
        if(ML >= 0) {
            if (checkLeft(freq))
                lpcp[ML] += fftmagdata[l] * fftmagdata[l];
            else
                rpcp[ML] += fftmagdata[l] * fftmagdata[l];
        }
    }

}
