#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCvMain.h"
#include "ofxKinect.h"
#include "ofxCv.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class audMode : public ofBaseApp {
public:
	
	void setup();
	void update(float vol);
	void draw();
	void exit();
	
	void drawPointCloud();
	
	float getValue(float volume);
	float mapValue(float value, float low, float high);
	void AudkeyPressed(int key);
	void AudmouseDragged(int x, int y, int button);
	void AudmousePressed(int x, int y, int button);
	void AudmouseReleased(int x, int y, int button);
	void AudwindowResized(int w, int h);
	
	ofxKinect kinect;
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	/*----------camera stuff----------------*/
	int angle;
	ofEasyCam easyCam; // used for viewing the point cloud
	/*----------------------------------------*/
	
	
	/*----------arduino stuffs----------------*/
	ofSerial serial;
	/*----------------------------------------*/
	
	float distance;
	int updates;
	ofColor choicecolor; 
	ofVec3f last_vec;
};
