#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "beatDetect.h"
#include "ofxColourTheory.h"

#include "alex/physicsMode.h"
#include "alex/vidMode.h"
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

		/*-----------util-----------*/
		int mouseX, mouseY;

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
		int externalBpm();
		void exit();

		ofxUICanvas *gui;
		float guiWidth, guiHeight;

		float slider2, DjDepthSliderHigh, DjDepthSliderLow; //change these after you decide what they're for
		bool drawDJ, drawAud, drawDisplay, drawSound;

		bool drawDJKinect, drawAudKinect;
		bool setDJ, setAud, setVid, setPhy, bGui;
		ofColor cmain, ccomp1, ccomp2, ccomp3, ccomp4, ccomp5, white;
		ofRectangle displayRect, djRect, audRect, guiRect;

		MODE mode;
		//int nearThresh, int farThresh;

		/*-----------Alex-----------*/
		//Physics
		physicsMode physics;
		physicsMode::source::Type sourceType;
		int numParticles;

		//Video
		vidMode vid;
		int vidX;
		int vidY;

		//Color Stuff
		vector<ofColor> colors;
		ofxColourTheory colorGen;
		ColourConstraints curShade;

		void generateColors(ColourShade seed);
		void drawColorSwatches(int x, int y);
		ColourShade IntelliColor();
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
		bool trackBeats(int low, int high); //check subbands between low and high for beats

		vector <float> left;
		vector <float> right;
		vector <float> volHistory;
		
		float cVol;
		float pVol;
		float lastBeatTime, startTime, lengthOfBeat, bpm;
		int tapCount;

		int AvgSetListBPM;

		ofSoundStream soundStream;
		ofxUIMovingGraph *audio;

		beatDetect bd;

	// 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)


		/*-----------Test Variables-----------*/
	int r,g,b;


};
