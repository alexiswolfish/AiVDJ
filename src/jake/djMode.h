#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "../cloth/ofxKCloth.h"


// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class djMode {
public:
	djMode();
	~djMode();

	void setup();
	void update(float depthLow, float depthHigh);
	void draw();
	void exit();

	void updatePoints();
	void drawPointCloud();
	void drawMeshCloud();
	void clothShit();

	void DJkeyPressed(int key);
	void DJmouseDragged(int x, int y, int button);
	void DJmousePressed(int x, int y, int button);
	void DJmouseReleased(int x, int y, int button);
	void DJwindowResized(int w, int h);
	void mouseMoved(int w, int h );
		
	bool WheresMyDj;
	int noDJ;
	float Zlow, Zhigh;
	vector<ofPolyline> lines;

	ofxKinect kinect;
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bDrawPointCloud;
	bool bDrawMeshCloud;
	bool bcloth;
	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;

	// cloth vars
	ClothController controller;
    float oldMouseX,oldMouseY;
    ofImage tex;
    ofShader shader;
	ofShader myShader;
    int rows, cols;
    ofLight directional;
};

