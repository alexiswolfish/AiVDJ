#include "djMode.h"
#include "ofAppGlutWindow.h"





//--------------------------------------------------------------
djMode::djMode(){

}

djMode::~djMode(){

}


void djMode::setup() {
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
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawMeshCloud = true;
	bDrawPointCloud = false;
	
}

//--------------------------------------------------------------
void djMode::update(float depthLow, float depthHigh) {
	
	ofBackground(100, 100, 100);
	
	kinect.update();

	Zlow = depthLow;
	Zhigh = depthHigh;
	

}

//--------------------------------------------------------------
void djMode::draw() {
	middleX = 320;
	
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} 
	else{
		easyCam.begin();
		drawMeshCloud();
		easyCam.end();
	}

}

void djMode::drawPointCloud() {
	maxY = ofVec3f(0,0,0);
	ofBackground(95, 100);

	//easyCam.setDistance(100);
	ofPushStyle();
	
	//ofSetColor(ofRandom(128, 255),ofRandom(128, 255),ofRandom(128, 255));	
	int rand1 = ofRandom(128, 255);
	int rand2 = ofRandom(128, 255);

	int w = 640;
	int h = 480;
	int step = 10;
	for(int y = ofRandom(0, 10); y < h; y += step) {
		for(int x = ofRandom(0, 10); x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				if (kinect.getWorldCoordinateAt(x, y).z < Zhigh && kinect.getWorldCoordinateAt(x, y).z > Zlow){	
					if (kinect.getWorldCoordinateAt(x, y).y > maxY.y){maxY= kinect.getWorldCoordinateAt(x, y);}
						ofPushMatrix();
						// the projected points are 'upside down' and 'backwards' 
						ofScale(1, -1, -1);
						ofTranslate(0, 0, -1000); // center the points a bit
						ofSetColor(rand1, rand2, kinect.getWorldCoordinateAt(x, y).z / (Zhigh/255));

						DJpoint newpoint;
						newpoint.x = kinect.getWorldCoordinateAt(x, y).x;
						newpoint.y = kinect.getWorldCoordinateAt(x, y).y;
						newpoint.z = kinect.getWorldCoordinateAt(x, y).z;
						DJpoints.push_back(newpoint);

						ofSphere(newpoint.x, newpoint.y, newpoint.z, 10);
						ofPopMatrix();
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

void djMode::drawMeshCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	//ofColor meshColor = ofColor(ofRandom(128.0, 255.0), 0, ofRandom(128.0, 255.0));
	int rand1 = ofRandom(128.0, 255.0);
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				if (kinect.getWorldCoordinateAt(x, y).z < Zhigh && kinect.getWorldCoordinateAt(x, y).z > Zlow){	
					mesh.addColor(ofColor(rand1, 0, (kinect.getWorldCoordinateAt(x, y).z*-1) / (Zhigh/255) + 128));
					mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
				}
			}
		}
	}

	ofBackground(95, 100);
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
}

//--------------------------------------------------------------
void djMode::DJkeyPressed (int key) {
	switch (key) {
			
		case'i':
			bDrawPointCloud = false;			
			bDrawMeshCloud = false;						break;

		case'p':
			if (bDrawPointCloud == bDrawMeshCloud){bDrawPointCloud=true;}
			bDrawPointCloud = !bDrawPointCloud;			
			bDrawMeshCloud = !bDrawMeshCloud;			break;
											
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
