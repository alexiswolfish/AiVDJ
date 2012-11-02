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
	void update(float depthLow, float depthHigh);
	void draw();
	void exit();
	
	void drawPointCloud();
	void drawMeshCloud();
	void testDraw();

	void DJkeyPressed(int key);
	void DJmouseDragged(int x, int y, int button);
	void DJmousePressed(int x, int y, int button);
	void DJmouseReleased(int x, int y, int button);
	void DJwindowResized(int w, int h);

	struct DJpoint {
		int x;
		int y;
		int z;
		ofVec3f location;
		ofColor color;
	};
	vector<DJpoint>DJpoints;

	float Zlow, Zhigh;
	ofVec3f maxY;
	int middleX;

	ofxKinect kinect;
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	bool bDrawMeshCloud;

	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
};

