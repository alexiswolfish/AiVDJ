#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS


class djMode {
public:
	djMode();
	~djMode();

	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();

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
	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
};
