/*
 *  audMode.h
 *  ofxKinectExample
 *
 *  Created by Melissa Alleyne on 10/23/12.
 *  Copyright 2012 Carnegie Mellon University. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
//#include "ofxCv.h"


class audMode {
public:
	audMode();
	~audMode();
	
	void setup();
	void update();
	void draw();
	void exit();
	
	ofColor getColor(int);
	float getNoiseColor(int, int);
	void drawPointCloud();
	
	void AudkeyPressed(int);
	void AudmouseDragged(int, int, int);
	void AudmousePressed(int, int, int);
	void AudmouseReleased(int, int, int);
	void AudwindowResized(int, int);
	
	
	ofxKinect kinect;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	int alpha;
	ofColor c0, c1, c2, c3, c4;
	// used for viewing the point cloud
	ofEasyCam easyCam;
};
