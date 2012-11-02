#include "audMode.h"
#include "ofAppGlutWindow.h"

//v1


//--------------------------------------------------------------
audMode::audMode(){
	
}

audMode::~audMode(){
	
}


void audMode::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	c1.setHex(0xF9CDAD); //butternut squash
	c2.setHex(0xFFD700); //cornucopia
	c3.setHex(0xFC9D9A); //peony
	c4.setHex(0x83AF9B); //Duck's Egg
	//c5.setHex(0xC8C8A9); //Timothy Hay
	alpha = 255;
	
	//
	//// enable depth->video image calibration
	//kinect.setRegistration(true);
 //   
	//kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 255;
	farThreshold = 255;
	
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 20;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;
}

//--------------------------------------------------------------
void audMode::update() {
	
	ofBackground(100, 100, 100);
	
	kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} 		
		// update the cv images
		grayImage.flagImageChanged();
		//hello	
	}
	
}

//--------------------------------------------------------------
void audMode::draw() {
	ofEnableAlphaBlending();
	ofBackground(50,1);
	ofDisableAlphaBlending();
	
	easyCam.begin();
	ofSetColor(0,255,255);
	drawPointCloud();
	easyCam.end();
	
	
}

void audMode::drawPointCloud() {	
	int w = 640;
	int h = 480;
	ofMesh mesh;
	ofMesh mesh_r;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	mesh_r.setMode(OF_PRIMITIVE_POINTS);
	int step = 3;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh_r.addColor(ofColor(255, 255, 255, alpha));
				mesh_r.addVertex(kinect.getWorldCoordinateAt(x, y));
				mesh.addColor(ofColor(230, 230, 230, alpha));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
				ofVec3f tmp = kinect.getWorldCoordinateAt(x, y);
				printf("%d %d %d\n", tmp.x, tmp.y, tmp.z );
			}
		}
	}
	glPointSize(1);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(-1, -1, -1);
	ofTranslate(w/2, 0, -1000); // center the points a bit
	//ofTranslate(0, 0, -1000); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	int counter = 0;
	mesh.drawVertices(); 
	//mesh.drawWireframe();
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
	
	
	glPointSize(1);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(-w/2, 0, -1000); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	mesh_r.drawVertices();
	//mesh_r.drawFaces ();
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix(); 

}

ofColor audMode::getColor(int x) {
	switch (x) {
		case 1:
			return c1;
			break;
		case 2:
			return c2;
			break;
		case 3:
			return c3;
			break;
		case 4:
			return c4;
			break;
		default:
			//return c5;
			break;
	}
}

//--------------------------------------------------------------
void audMode::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
}

//--------------------------------------------------------------
void audMode::AudkeyPressed (int key) {
	switch (key) {
		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void audMode::AudmouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void audMode::AudmousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void audMode::AudmouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void audMode::AudwindowResized(int w, int h)
{}
