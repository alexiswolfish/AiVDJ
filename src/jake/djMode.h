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
	void update(vector<float> &vol, float depthLow, float depthHigh);
	void draw();
	void exit();

	// cloth vars
	ClothController controller;
    float oldMouseX,oldMouseY;
    ofImage tex;
    ofShader shader;
	ofShader myShader;
    int rows, cols;
    ofLight directional;

	void updatePoints();
	void updateGlobals(ofColor c, bool changeColor);
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
	int noDJ, tiltDegr;
	float Zlow, Zhigh, maxY;

	ofVec3f maxPt;
	vector<ofPolyline> lines;
	vector<float> volHist;
	ofColor smartColor;

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


};

