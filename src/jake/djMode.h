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
	void update(vector<float> &vol, float depthLow, float depthHigh, bool beat);
	void draw();
	void exit();

	//CV shit 
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	int nearThreshold;
	int farThreshold;
	vector<ofPoint> fingers;
	ofPoint palmCenter;

	ofxCvContourFinder contourFinder;

	// cloth vars
	ClothController controller;
    float oldMouseX,oldMouseY;
    ofImage tex;
    ofShader shader;
	ofShader myShader;
    int rows, cols;
    ofLight directional;

	void updatePoints();
	void updateGlobals(ofColor c, bool changeColor, float volume);
	vector<ofPoint> getFingerTips( ofxCvGrayscaleImage input);
	void drawPointCloud();
	void drawMeshCloud();
	void clothShit();

	void drawImage();

	bool newBeat;

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
	ofMesh mesh;

	vector<float> volHist;
	vector<vector<float>> volHistVec;
	ofColor smartColor;
	float volf;

	void drawCircle(float radius, ofColor _color, vector<float> v);
	void drawCircleBg();

	ofxKinect kinect;

	//image shit
	ofImage albumArt;
	ofxCvGrayscaleImage BnW_image, wave_image;
//	ofPixelsRef newPixs;
	float waves(float x, float y, float w, float a, float t);
	bool wave;
	int wave_count;
	//float new_img_height;
	
	bool bDrawPointCloud;
	bool bDrawMeshCloud;
	bool bcloth;
	bool init_cloth;
	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;

	float maxLines;
};

