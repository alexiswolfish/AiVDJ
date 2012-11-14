#ifndef _LIVE_POEM
#define _LIVE_POEM

#include "ofMain.h"
#include "fft.h"

#include <list>

#define FFT_BINS 512
#define FFT_SUBBANDS 32
#define ENERGY_HISTORY 43

using namespace std;

class beatDetect
{
protected:
    // fft & beat detection
	float fftSmoothed[FFT_BINS]; // averaged fft
	float fftSubbands[FFT_SUBBANDS]; // raw value of each subband
	float averageEnergy[FFT_SUBBANDS]; // average energy of the subband
    float fftVariance[FFT_SUBBANDS]; // variance of energy
    float beatValueArray[FFT_SUBBANDS]; // constants used for beat detection
	float energyHistory[FFT_SUBBANDS][ENERGY_HISTORY]; //history of the subband energy
	float *in_fft;
	float beatValue;
	int historyPos;
    
	
	bool fftInit;
	ofSoundPlayer soundtrack;
	
public:
	beatDetect();
	virtual ~beatDetect() {}

    // FFT functions
	void initFFT();
    void updateFFT();
    //void updateFFT(float* in_fft, int infft_size);
    void drawSmoothedFFT();
    void drawSubbands();
    //void drawBeats();
    void audioReceived(float* input, int bufferSize);
    
    // detect beats
    void enableBeatDetect() {bDetectBeat = true;}
    void disableBeatDetect() {bDetectBeat = false;}
    bool isBeat(int subband);
    bool isKick();
    bool isSnare();
    bool isHat();
    bool isBeatRange(int low, int high, int threshold);
    void setBeatValue(float bv) {beatValue = bv;}

    // fft object
    float *magnitude, *phase, *power, *audio_input;
    float *magnitude_average, *magnitude_average_snapshot; 
	bool bDetectBeat;
    fft myfft;
    
};

#endif
