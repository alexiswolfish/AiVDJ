#include "beatDetect.h"
#include "fft.h"
// FFT BIN = 256
// FFT_SUBBANDS  32 -> subbands
// ENERGY_HISTORY  43 -> energyHistory


int fft_size=512;
int buffer_size = 1024;

beatDetect::beatDetect()
{
    for(int i = 0; i < fft_size; i++)
        fftSmoothed[i] = 0;

    // history of energy, 42 to 32 pieces each frequency subband
    for(int i = 0; i < FFT_SUBBANDS; i++)
    {
        for(int l = 0; l < ENERGY_HISTORY; l++){
            energyHistory[i][l] = 0;
        }
        fftSubbands[i] = 0;
        averageEnergy[i] = 0;
        fftVariance[i] = 0;
        beatValueArray[i] = 0;
    }

	audio_input = new float[buffer_size];
	magnitude = new float[fft_size];
	phase = new float[fft_size];
	power = new float[fft_size];
	magnitude_average = new float[fft_size];
	magnitude_average_snapshot = new float[fft_size];

	for (int i = 0; i < fft_size; i++) {
		magnitude[i] = 0;
		phase[i] = 0;
		power[i] = 0;
		magnitude_average_snapshot[i] = 0;
		magnitude_average[i] = 0;
	}

    historyPos = 0; 
    fftInit = true;
    beatValue = 1.08;
    enableBeatDetect();
    printf("beatDetect setup OK!! \n");
}

void beatDetect::updateFFT()
{
    if(fftInit)
    {
        // pass aray to fft mag obtained in audio recieved
        in_fft = magnitude;
        for (int i = 0; i < fft_size; i++)
        {
            // take the max, either the smoothed or the incoming:
            // update the value is greater than 0.9 times the
            if (fftSmoothed[i] < in_fft[i]) 
                fftSmoothed[i] = in_fft[i];
            // let the smoothed value sink to zero:
            // Set to 0.9 times the amplitude value of the acquired frequency, we gradually reduced to zero
            // eliminate influence of old amp values
            fftSmoothed[i] *= 0.90f;
        }

        if(bDetectBeat) //only if initialized
        {
            // calculate sub-band
            // This source is the number of sub-bands in 32 -> loop 32 
            for(int i = 0; i < FFT_SUBBANDS; i++)
            {
                fftSubbands[i] = 0;
                // Loop 256/32 = 8
                for(int b = 0; b < fft_size/FFT_SUBBANDS; b++) {
                    // [I * 8 + 1 ~ 8] -> in increments of 8, 7,8 - 0 to the sum in the sense> of 23 -> 15 and 16
                    fftSubbands[i] +=  in_fft[i*(fft_size/FFT_SUBBANDS)+b];
                }
				// After taking the sum of the values ​​of the sub-band, the average value obtained by adding the number of times I split
                fftSubbands[i] = fftSubbands[i]*(float)FFT_SUBBANDS/(float)fft_size;
                
                // calc dispersion of sub-bands
                for(int b=0; b < fft_size/FFT_SUBBANDS; b++)
                    fftVariance[i] += pow(in_fft[i*(fft_size/FFT_SUBBANDS)+b] - fftSubbands[i], 2);
                fftVariance[i] = fftVariance[i]*(float)FFT_SUBBANDS/(float)fft_size;
                
                //change the value dynamically, balance constant C
                beatValueArray[i] = (-0.0025714*fftVariance[i])+1.35;
            }

            // average energy calculation
            for(int i = 0; i < FFT_SUBBANDS; i++) {
                averageEnergy[i] = 0;
                for(int h = 0; h < ENERGY_HISTORY; h++) {
                    // energy history is the average total of each subband
                    averageEnergy[i] += energyHistory[i][h];
                }
                // weight the average
                averageEnergy[i] /= ENERGY_HISTORY;
            }

            // put new values into energy history
            // 32回loop
            for(int i = 0; i < FFT_SUBBANDS; i++) {
                // add to the history of the energy position of the sub-band historyPos calculated
                energyHistory[i][historyPos] = fftSubbands[i];
            }
            // artificial circulation by subscript of the array
            historyPos = (historyPos+1) % ENERGY_HISTORY; // forward pointer and rotate if necessary
        }
    }
	getVolume();
}

float beatDetect::getVolume(){
	pVol = curVol;
	float fft_bins = 512.0f; //this really should be a class constant
	float cVol = 0;
	for(int i=0; i<fft_bins; i++)
		cVol += magnitude_average[i];
	cVol/=fft_bins;
	curVol = cVol;
	return cVol;
}

bool beatDetect::diffVol(float thresh){
	return abs(curVol-pVol) > thresh;
}

//When you receive a voice input, update the array magnitude FFT analysis is performed
void beatDetect::audioReceived(float* input, int bufferSize){
	memcpy(audio_input, input, sizeof(float) * bufferSize);
	
	float avg_power = 0.0f;
	
	//check specturm, value of the FFT amp wanted magnitude
	myfft.powerSpectrum(0, (int)fft_size, audio_input, buffer_size, 
						magnitude, phase, power, &avg_power);
	
	// tone down magnitude
	for (int i = 0; i < fft_size; i++) {
		magnitude[i] = powf(magnitude[i], 0.5);
	}

	// calc average value of the FFt amplitude (volume)
	for (int i = 0; i < fft_size; i++) {
		float x = 0.085;
		magnitude_average[i] = (magnitude[i] * x) + (magnitude_average[i] * (1 - x));
	}
}

// decide the width of the sub bands used in the detection
bool beatDetect::isBeatRange(int low, int high, int threshold)
{
    int num = 0;
    for(int i = low; i < high+1; i++) 
        if(isBeat(i)) 
            num++;
    return num > threshold;

}

// Beat itself
bool beatDetect::isBeat(int subband)
{
    return fftSubbands[subband] > averageEnergy[subband]*beatValueArray[subband];
}

bool beatDetect::isKick()
{
    return isBeat(0);
}

bool beatDetect::isSnare()
{
    int low = 1;
    int hi = FFT_SUBBANDS/3;
    int thresh = (hi-low)/3;
    return isBeatRange(low, hi, thresh);
}

bool beatDetect::isHat()
{
    int low = FFT_SUBBANDS/2;
    int hi = FFT_SUBBANDS-1;
    int thresh = (hi-low)/3;
    return isBeatRange(low, hi, thresh);
}

void beatDetect::drawSmoothedFFT(){
	int height = 75;
	int width = FFT_SUBBANDS*3+10;
	int spacer = 16;

	for(int i=0; i<FFT_SUBBANDS; i++){
		ofLine(10+(i*3),height,10+(i*3),height-fftSmoothed[i]*20.0f);
	}

}
//the magnitude(volume) of each frequency
void beatDetect::drawAverageMagnitude(){
	float rectWidth = 512;
	float rectHeight = 150;
	float spacer = 16;
	ofPushStyle();
	for (int i = 1; i < (int)rectWidth/2; i++){
        if(i % 16 == 0)
            ofSetHexColor(0xf56494);
        else
            ofSetColor(255,255,255);
		ofLine(10+(i*3), 0,  10+(i*3), magnitude_average[i]*10.0f);
	}
	ofPopStyle();
}
//draw the three other graphs
void beatDetect::drawSubbands(){
	ofPushMatrix();
	int height = 75;
	int width = FFT_SUBBANDS*3+10;
	int spacer = 16;
	ofDrawBitmapString("Fft Subbands", 0, 0);
	for(int i=0; i<FFT_SUBBANDS; i++){
		ofLine(10+(i*3),height,10+(i*3),height-fftSubbands[i]*20.0f);
	}
	ofTranslate(width+spacer,0,0);
	ofDrawBitmapString("Average Energy", 0, 0);
	for(int i=0; i<FFT_SUBBANDS; i++){
		ofLine(10+(i*3),height,10+(i*3),height-averageEnergy[i]*20.0f);
	}
	ofTranslate(width+spacer,0,0);
	ofDrawBitmapString("Fft Variance", 0, 0);
	for(int i=0; i<FFT_SUBBANDS; i++){
		ofLine(10+(i*3),height,10+(i*3),height-fftVariance[i]*20.0f);
	}
	ofPopMatrix();
}

void beatDetect::drawBeats(){

	int height = 75;
	int width = FFT_SUBBANDS*3+10;
	int spacer = 16;
	ofPushStyle();
	for(int i=0; i<FFT_SUBBANDS; i++){
		if(isBeat(i))
			ofSetHexColor(0xf56494);
		else
			ofSetColor(255,255,255);
		ofLine(10+(i*3),height,10+(i*3),height-fftSubbands[i]*20.0f);
	}
	ofPopStyle();
}

