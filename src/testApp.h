#pragma once

#include "ofMain.h"
#include "ofxUI.h"

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
		void guiSetup();
		void guiColors(ofxUIWidget *w);
		void guiEvent(ofxUIEventArgs &e);
		void initRects();
		void exit();

		ofxUICanvas *gui;
		float guiWidth;
		float slider1, slider2; //change these after you decide what they're for
		bool drawDJ, drawAud, drawDisplay;
		ofColor cmain, ccomp1, ccomp2, ccomp3, ccomp4, ccomp5;
		ofRectangle displayRect, djRect, audRect, guiRect;

		//int nearThresh, int farThresh;
		/*-----------Jake-----------*/
		djMode DJ;

		/*-----------Test Variables-----------*/
	int r,g,b;
	
		/*-----------Melissa-----------*/
	audMode Aud;
};
