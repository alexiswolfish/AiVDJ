#include "djMode.h"
#include "ofAppGlutWindow.h"

//graphics shader example



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
	
	//kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	kinect.open();
	//kinect.open("B00363262039047B");	// open a kinect using it's unique serial #
	
	colorImg.allocate(kinect.width, kinect.height);
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 10;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawMeshCloud = true;
	bDrawPointCloud = false;
	//printf("serial:'%s'", kinect.getSerial());
	easyCam.tilt(30);
	
}

//--------------------------------------------------------------
void djMode::update(float depthLow, float depthHigh, float newslider) {
	
	ofBackground(100, 100, 100);
	
	kinect.update();

	Zlow = depthLow;
	Zhigh = depthHigh;
	testVar = newslider;
	

}

//--------------------------------------------------------------
void djMode::draw() {
	middleX = 320;
	
	easyCam.begin();
	if(bDrawPointCloud) {
		drawPointCloud();
	} 
	else{
		drawMeshCloud();
	}
	easyCam.end();

}

void djMode::drawPointCloud() {
	//maxY = ofVec3f(0,0,0);
	ofBackground(95, 100);

	//easyCam.setDistance(100);
	ofPushStyle();
	
	//ofSetColor(ofRandom(128, 255),ofRandom(128, 255),ofRandom(128, 255));	
	int rand1 = ofRandom(128, 255);
	int rand2 = ofRandom(128, 255);

	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	
	vector<ofVec3f>lastVert;
	int w = 640;
	int h = 480;
	int step = 10;
	for(int y = 0; y < h; y += step) {
		ofPolyline line;
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				if (kinect.getWorldCoordinateAt(x, y).z < Zhigh && kinect.getWorldCoordinateAt(x, y).z > Zlow){	
					ofSetColor(rand1, rand2, kinect.getWorldCoordinateAt(x, y).z / (Zhigh/255));
					ofSphere(kinect.getWorldCoordinateAt(x, y).x, kinect.getWorldCoordinateAt(x, y).y, kinect.getWorldCoordinateAt(x, y).z, 1);
					if (lastVert.size() > 0){
						float dist = ofDistSquared(kinect.getWorldCoordinateAt(x, y).x,kinect.getWorldCoordinateAt(x, y).y,lastVert.at(lastVert.size()-1).x,lastVert.at(lastVert.size()-1).y);
						float Ydist = ofDistSquared(0,kinect.getWorldCoordinateAt(x, y).y,0,lastVert.at(lastVert.size()-1).y);
						if (dist < testVar && Ydist < step*step){
							printf("\ndistance? %f %f %f %f %f \n",dist,kinect.getWorldCoordinateAt(x, y).x,kinect.getWorldCoordinateAt(x, y).y,lastVert.at(lastVert.size()-1).x,lastVert.at(lastVert.size()-1).y);
							line.addVertex(kinect.getWorldCoordinateAt(x, y).x, kinect.getWorldCoordinateAt(x, y).y, kinect.getWorldCoordinateAt(x, y).z);
						}
					}
					lastVert.push_back(kinect.getWorldCoordinateAt(x, y));
				}
			}
		}
		line.close();
		//line.quadBezierTo(line[0],line[line.size()/2], line[line.size()-1]);
		line.draw();
	}
	
	ofPopMatrix();

	//if (maxY >= 320 && maxY < 480){
	//	easyCam.tilt(0);
	//	easyCam.tilt(-maxY-320/3.55);  //max tilt ~-45
	//	return -maxY-320/3.55;
	//}
	//else return 0.0;
	ofPopStyle();
}

void djMode::drawMeshCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 5; // try higher steps 
	//ofColor meshColor = ofColor(ofRandom(128.0, 255.0), 0, ofRandom(128.0, 255.0));
	int rand1 = ofRandom(128.0, 255.0);
	for(int y = 0; y < h; y += step*1.5) {
		ofPolyline line;
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				if (kinect.getWorldCoordinateAt(x, y).z < Zhigh && kinect.getWorldCoordinateAt(x, y).z > Zlow){	
					mesh.addColor(ofColor(rand1, 0, (kinect.getWorldCoordinateAt(x, y).z*-1) / (Zhigh/255) + 128));
					mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
				}
			}
		}
		line.close();
		line.draw();
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
