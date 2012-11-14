#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "beatDetect.h"
#include "ofxColourTheory.h"

#include "alex/physicsMode.h"
#include "jake/djMode.h"
#include "melissa/audMode.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		/*-----------gui-----------*/
		enum MODE{
			DJ,
			AUD,
			PHYSICS,
			VID
		};

		void guiSetup();
		void guiColors(ofxUIWidget *w);
		void guiEvent(ofxUIEventArgs &e);
		void initRects();
		void exit();

		ofxUICanvas *gui;
		float guiWidth, guiHeight;

		float slider2, DjDepthSliderHigh, DjDepthSliderLow; //change these after you decide what they're for
		bool drawDJ, drawAud, drawDisplay, drawSound;

		bool drawDJKinect, drawAudKinect;
		ofColor cmain, ccomp1, ccomp2, ccomp3, ccomp4, ccomp5, white;
		ofRectangle displayRect, djRect, audRect, guiRect;

		MODE mode;
		//int nearThresh, int farThresh;

		/*-----------Alex-----------*/
		physicsMode physics;
		physicsMode::source::Type sourceType;
		int numParticles;

		vector<ofColor> colors;
		ofxColourTheory colorGen;

		void generateColors(ofColor seed);
		/*-----------Jake-----------*/
		djMode DJMODE;
		/*-----------Melissa-----------*/
		audMode Aud;


		/*-----------Sound-----------*/
		/*
			sub bass : 0 > 100hz
			mid bass : 80 > 500hz
			mid range: 400 > 2khz
			upper mid: 1k > 6khz
			high freq: 4k > 12khz
			Very high freq: 10k > 20khz and above
		*/

		void audioIn(float * input, int bufferSize, int nChannels); 

		void drawVolGraphs();
		void drawBeatBins();

		vector <float> left;
		vector <float> right;
		vector <float> volHistory;
		
		float cVol;
		float pVol;

		ofSoundStream soundStream;
		ofxUIMovingGraph *audio;

		beatDetect bd;
		
	// 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)

	
};
