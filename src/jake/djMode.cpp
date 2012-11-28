#include "djMode.h"
#include "ofAppGlutWindow.h"

//--------------------------------------------------------------
djMode::djMode(){

}

djMode::~djMode(){

}


void djMode::setup() {

	WheresMyDj = true;
	noDJ = 0;
	bDrawMeshCloud = false;
	bDrawPointCloud = true;
	bcloth = false;
	
	vector<int>sortedY(640, 0);

	ofSetLogLevel(OF_LOG_VERBOSE);
	//// enable depth->video image calibration
	kinect.setRegistration(true);

	kinect.init();
	kinect.init(true); // shows infrared instead of RGB video image
	kinect.init(false, false); // disable video image (faster fps)

	kinect.open();
	////kinect.open("B00363262039047B");	// open a kinect using it's unique serial #
	//ofBackground(100, 100, 100);
	ofSetFrameRate(60);

	if (bcloth){
		ofEnableSmoothing();
		cols = 640 / CLOTH_RES;
		rows = 480 / CLOTH_RES; 
		controller.init(cols,rows);
		controller.initMesh(); 
		tex.loadImage("large_brown.png"); 
		shader.load("shader");
		directional.setDirectional();
		oldMouseX = -999;
		oldMouseY = -999;
	}
	angle = 10;
	kinect.setCameraTiltAngle(angle);
	//printf("serial:'%s'", kinect.getSerial());
	easyCam.tilt(15);
	

}

//--------------------------------------------------------------
void djMode::update(vector<float> &vol, float depthLow, float depthHigh) {
	//ofBackground(100, 100, 100);
	
	kinect.update();

	Zlow = depthLow;
	Zhigh = depthHigh;

	if (bcloth) clothShit();
	else if (bDrawPointCloud){
		int w = 640;
		int h = 480;
		int step = 5;
		bool thresh = false;
		int points = 0;
		for(int y = 0; y < h; y += step) {
			for(int x = 0; x < w; x += step) {
				if(kinect.getDistanceAt(x, y) > 0) {
					if (kinect.getWorldCoordinateAt(x, y).z < Zhigh && kinect.getWorldCoordinateAt(x, y).z > Zlow){	
						points++;
						ofVec3f vec = kinect.getWorldCoordinateAt(x, y);
						if (!thresh){
							thresh = true;
							ofPolyline p;
							p.addVertex(vec.x, vec.y + (vol[x] * 1200.0f), vec.z);
							lines.push_back(p);
						}
						else{
							lines.back().addVertex(vec.x, vec.y + (vol[x] * 1200.0f), vec.z);
						}
					}
					else{
						thresh = false;
					}
				}
			}
		}
		if (points < 750){
			noDJ++;
			//printf("\nnodj %d\n", noDJ);
		}
		if (noDJ > 10){
			WheresMyDj = false;
			noDJ = 0;
		}
	}

}

//--------------------------------------------------------------
void djMode::updateGlobals(ofColor c, bool changeColor) {
	if (changeColor)smartColor = c;
}

//--------------------------------------------------------------


void djMode::draw() {
	//ofBackground(95, 100);
	
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} 
	else if (bDrawMeshCloud){
		easyCam.begin();
		drawMeshCloud();
		easyCam.end();
	}
	else if (bcloth){
		ofPushMatrix();
		glEnable(GL_DEPTH_TEST);
		ofTranslate(ofGetWidth()/2-cols*CLOTH_RES/2, 100,0);
		shader.begin();
		shader.setUniformTexture("tex", tex.getTextureReference(), 0);
		controller.drawMesh();
		shader.end();
		ofPopMatrix();
	}
}



void djMode::drawPointCloud() {
	ofPushMatrix();  //comment out??
	ofScale(1, -1, -1); 
	ofTranslate(0, 0, -1000); // center the points a bit

	ofBackground(95, 100);
	ofPushStyle();
	ofSetColor(smartColor);
	for (int i=0; i<lines.size();i++){
		lines[i].draw();
	}
	//if (maxY >= 320 && maxY < 480){
	//	easyCam.tilt(0);
	//	easyCam.tilt(-maxY-320/3.55);  //max tilt ~-45
	//	return -maxY-320/3.55;
	//}
	//else return 0.0;
	ofPopStyle();
	ofPopMatrix();
	lines.clear();
}

void djMode::drawMeshCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	float maxY = 0;
	float minX = 0;
	float maxX = 0;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 4; // try higher steps 
	int rand1 = ofRandom(128.0, 255.0);
	for(int y = 0; y < h; y += step*1.5) {
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
	if (mesh.getVertices().size() < 750){
		noDJ++;
		//printf("\nnodj %d\n", noDJ);
	}
	if (noDJ > 10){
		WheresMyDj = false;
		noDJ = 0;
	}

	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
	
}

void djMode::clothShit(){
	for(int i=0;i<cols*rows;i++) {
 
			int x = int(i) % cols;
			int y = int(i) / cols;
        
			if(y == cols-1) continue;
              
			float d = kinect.getDistanceAt(x*CLOTH_RES, y*CLOTH_RES);

			if(d >0 && d < 1000) {
            
				d = ofMap(d,0,4000,0,30);
                            
				ofVec3f ff = ofVec3f(0,0.0,d);
				ff.normalize();
				ff *= 3.0;
                  
				controller.particles[i]->addForce(ff);
				
		   }
				if (controller.particles.size() < 500){
					noDJ++;
					//printf("\nnodj %d\n", noDJ);
				}
				if (noDJ > 10){
					WheresMyDj = false;
					noDJ = 0;
				}
		}
   
		controller.update();
		controller.updateMesh();
		controller.updateMeshNormals();
}

//--------------------------------------------------------------
void djMode::exit() {
	//kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void djMode::DJkeyPressed (int key) {
	switch (key) {
			
		case'i':
			bDrawPointCloud = false;			
			bDrawMeshCloud = false;			break;

		case'p':
			if (bDrawMeshCloud){
				//bDrawPointCloud=false;
				bcloth = true;
				bDrawMeshCloud = false;
			}
			else{
				bcloth = false;
				bDrawMeshCloud = true;
			}
			//bDrawPointCloud = !bDrawPointCloud;			
			//bDrawMeshCloud = !bDrawMeshCloud;			
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
{
    ofVec3f f = ofVec3f(0,0,50);
    
    controller.particles[200]->addForce(f);
}

//--------------------------------------------------------------
void djMode::DJmouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void djMode::DJwindowResized(int w, int h)
{}

void djMode::mouseMoved(int x, int y ){
    directional.setPosition(x, y, 100);
    
}

