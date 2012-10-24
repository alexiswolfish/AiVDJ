#include "djMode.h"
#include "ofAppGlutWindow.h"





//--------------------------------------------------------------
djMode::djMode(){

}

djMode::~djMode(){

}


void djMode::setup(float depth) {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	kinect.init(true); // shows infrared instead of RGB video image
	kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 255;
	farThreshold = 202;
	bThreshWithOpenCV = true;
	Zdepth = depth;

	scaleX = 1024/640;
	scaleY = 768/480;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawMeshCloud = true;
	bDrawPointCloud = false;
	
}

//--------------------------------------------------------------
void djMode::update(float depth) {
	
	ofBackground(100, 100, 100);
	
	kinect.update();

	Zdepth = depth;
	
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
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			unsigned char * pix = grayImage.getPixels();
			
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void djMode::draw() {
	middleX = 320;
	
	ofSetColor(255, 255, 255);
	
	if(bDrawPointCloud) {
		easyCam.begin();
		testDraw();
		easyCam.end();
	} 
	else if(bDrawMeshCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	}else {
		// draw from the live kinect
		kinect.drawDepth(10, 10, 400, 300);
		kinect.draw(420, 10, 400, 300);
		
		grayImage.draw(10, 320, 400, 300);
		contourFinder.draw(10, 320, 400, 300);
		
#ifdef USE_TWO_KINECTS
		kinect2.draw(420, 320, 400, 300);
#endif
	}
	
	if (!bDrawMeshCloud && !bDrawPointCloud){
		//draw instructions
		ofSetColor(0, 255, 0);   //green
		stringstream reportStream;
		reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
		<< ofToString(kinect.getMksAccel().y, 2) << " / "
		<< ofToString(kinect.getMksAccel().z, 2) << endl
		<< "press i to enter intialize kinect mode" << endl
		<< "press p to switch between point cloud modes" << endl
		<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
		<< "set near threshold " << nearThreshold << " (press: + -)" << endl
		<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
		<< ", fps: " << ofGetFrameRate() << endl
		<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
		<< "max Y: " << maxY.y << endl
		<< "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl;
		ofDrawBitmapString(reportStream.str(),20,652);
	}

}

void djMode::testDraw() {
	maxY = ofVec3f(0,0,0);
	ofBackground(95, 100);

	//easyCam.setDistance(100);
	ofPushStyle();
	ofSetColor(ofRandom(128, 255),ofRandom(128, 255),ofRandom(128, 255));
	int w = 640;
	int h = 480;
	int step = 10;
	for(int y = ofRandom(0, 10); y < h; y += step) {
		for(int x = ofRandom(0, 10); x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				if (kinect.getWorldCoordinateAt(x, y).z < Zdepth){	
					if (kinect.getWorldCoordinateAt(x, y).y > maxY.y){maxY= kinect.getWorldCoordinateAt(x, y);}
					
					DJpoint newpoint;
					newpoint.x = kinect.getWorldCoordinateAt(x, y).x * scaleX;
					newpoint.y = -kinect.getWorldCoordinateAt(x, y).y * scaleY;
					newpoint.z = -kinect.getWorldCoordinateAt(x, y).z;
					DJpoints.push_back(newpoint);
					ofSphere(newpoint.x, newpoint.y, newpoint.z, 10);
				}
			}
		}
	}
	//ofSetColor(0,255,255);
	//ofSphere(maxY.x, -maxY.y, -maxY.z, 50);
	
	//for (int i=0; i < DJpoints.size(); i++){
	//	if(i != 0 && DJpoints[i].y == DJpoints[i-1].y){
	//		ofSetLineWidth(5);
	//		ofLine(DJpoints[i].x,DJpoints[i].y,DJpoints[i].z,DJpoints[i-1].x,DJpoints[i-1].y,DJpoints[i-1].z);
	//	}
	//}

	//if (maxY >= 320 && maxY < 480){
		//easyCam.tilt(0);
		//easyCam.tilt(-maxY-320/3.55);  //max tilt ~-45
		//return -maxY-320/3.55;
	//}
	//else return 0.0;
	ofPopStyle();
	DJpoints.clear();
}

void djMode::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	ofColor meshColor = ofColor(ofRandom(128.0, 255.0), 0, ofRandom(128.0, 255.0));
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				if (kinect.getWorldCoordinateAt(x, y).z < Zdepth){
					mesh.addColor(meshColor);
					mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
				}
			}
		}
	}


	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
}

//--------------------------------------------------------------
void djMode::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void djMode::DJkeyPressed (int key) {
	switch (key) {
		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;
			
		case'i':
			bDrawPointCloud = false;			
			bDrawMeshCloud = false;						break;

		case'p':
			if (bDrawPointCloud == bDrawMeshCloud){bDrawPointCloud=true;}
			bDrawPointCloud = !bDrawPointCloud;			
			bDrawMeshCloud = !bDrawMeshCloud;			break;
			
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
void djMode::DJmouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void djMode::DJmousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void djMode::DJmouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void djMode::DJwindowResized(int w, int h)
{}
