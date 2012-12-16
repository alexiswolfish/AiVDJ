#include "djMode.h"
#include "ofAppGlutWindow.h"

//--------------------------------------------------------------
djMode::djMode(){

}

djMode::~djMode(){

}


void djMode::setup() {

	ofSetLogLevel(OF_LOG_VERBOSE);

	WheresMyDj = true;
	noDJ = 0;
	bDrawMeshCloud = false;
	bDrawPointCloud = true;
	tiltDegr = 15;

/*	albumArt.loadImage("data/album_art/common_be.jpg");
	albumArt.setImageType(OF_IMAGE_GRAYSCALE);
	BnW_image.setFromPixels(albumArt.getPixels(),albumArt.width,albumArt.height);
	BnW_image.threshold(50);*/	

	//// enable depth->video image calibration
	kinect.setRegistration(true);

	kinect.init();
	kinect.init(true); // shows infrared instead of RGB video image
	kinect.init(false, false); // disable video image (faster fps)

	kinect.open();
	////kinect.open("B00363262039047B");	// open a kinect using it's unique serial #
	//ofBackground(100, 100, 100);
	ofSetFrameRate(60);

	angle = 0;
	kinect.setCameraTiltAngle(angle);
	//printf("serial:'%s'", kinect.getSerial());
	//easyCam.tilt(15);

	maxLines = 2;
}

//--------------------------------------------------------------
void djMode::update(vector<float> &vol, float depthLow, float depthHigh, bool beat) {

	kinect.update();

	//for (int i=0; i<=360; i++){
	//	volHist.push_back(vol[i]);
	//}

	//volHistVec.push_back(volHist);
	//for (int i=volHistVec.size(); i>0; i--){
	//	if(i!=volHistVec.size()) volHistVec[i+1] = volHistVec[i];
	//}
	//if (volHistVec.size() > 10){
	//	volHistVec.pop_back();
	//}



	Zlow = depthLow;
	Zhigh = depthHigh;
	newBeat = beat;

	if(kinect.isFrameNew()) {
		int w = 640;
		int h = 480;
		int step = 2;
		bool thresh = false;
		int points = 0;
		//maxY = 0;
		for(int y = 0; y < h; y += step*5) {
			for(int x = 0; x < w; x += step) {
				if(kinect.getDistanceAt(x, y) > 0) {
					if (kinect.getWorldCoordinateAt(x, y).z < Zhigh && kinect.getWorldCoordinateAt(x, y).z > Zlow){	
						points++;
						ofVec3f vec = kinect.getWorldCoordinateAt(x, y);
						float addMe = (vol[x] * 800.0f);
						if (addMe > 5) addMe = 5; 
						else if (addMe < -5) addMe = -5;

						if (vec.y > maxY) maxY = vec.y;
						if (!thresh){
							thresh = true;
							ofPolyline p;
							p.addVertex(vec.x, vec.y + addMe, vec.z);
							lines.push_back(p);
						}
						else{
							lines.back().addVertex(vec.x, vec.y + addMe, vec.z);
						}
					}
					else{
						thresh = false;
					}
				}
			
			}
		}
		if(lines.size() > maxLines)
			lines.erase(lines.begin());


			//if (beat){
			//	mesh.setMode(OF_PRIMITIVE_POINTS);
			//	int rand1 = ofRandom(128.0, 255.0);
			//	for(int y = 0; y < h; y += step*2) {
			//		for(int x = 0; x < w; x += step*2) {
			//			if(kinect.getDistanceAt(x, y) > 0) {
			//				if (kinect.getWorldCoordinateAt(x, y).z > Zhigh){	
			//					float sizeD = (vol[x] * 2000.0f);
			//					if (sizeD > 5) sizeD = 5; 
			//					else if (sizeD < -5) sizeD = -5;
			//					mesh.addColor(ofColor(rand1, 0, (kinect.getWorldCoordinateAt(x, y).z*-1) / (Zhigh/255) + 128));
			//					mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			//					//bg_points.point.push_back(kinect.getWorldCoordinateAt(x, y));
			//					//bg_points.color.push_back(ofColor(rand1, 0, (kinect.getWorldCoordinateAt(x, y).z*-1) / (Zhigh/255) + 128));
			//					//bg_points.size.push_back(sizeD);
			//				}
			//			}
			//		}
			//	}
			//}

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
void djMode::updateGlobals(vector<ofColor> cols, bool changeColor, float volume) {
	smartColors = cols;
	smartDarkCol = colorGen.getDarkest(cols);
	newColor = smartDarkCol;
	newColor.setBrightness(smartDarkCol.getBrightness() - 20);
	if (changeColor) smartColor =  colorGen.getRandom(cols);
		
	volf = volume;
}


//--------------------------------------------------------------


void djMode::draw() {

	//ofSetColor(95, 165);
	ofSetColor(smartDarkCol);
	ofRect(0,0,ofGetWidth(), ofGetHeight());
	//ofBackground(95, 100);

	ofPushStyle();
	ofPushMatrix();
	if(bDrawPointCloud) {
		drawCircleBg();
		//drawImage();
		//int k = 50;
		//if (volHistVec.size() > 9){
		//	int size = 9;
		//	for (int i=750; i>200; i-= 50){
		//		vector<float> h = volHistVec[size];
		//		drawCircle(i,ofColor(k),h);
		//		size--;
		//		k += 10;
		//	}
		//}
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
				
	} 

	ofPopStyle();
	ofPopMatrix();


}

void djMode::drawCircleBg(){
	float radius = 700;
	int k = 70;
	ofVec2f center = ofVec2f(ofGetWidth()/2, ofGetHeight()/2);
	ofPushStyle();
	ofPushMatrix();
	for(int i=0; i< lines.size(); i++){
		ofPolyline curLine = lines[i];
		vector<ofPoint> points = curLine.getVertices();
		ofRectangle bb = curLine.getBoundingBox();
		
		ofPolyline circle;
		for (int i=0; i<points.size(); i+=25){
			float dist = ofDist(0, points[i].y,0, bb.getMinY());
			float rad = ofRandom(0, TWO_PI);
			float r = (radius + (dist*.25));
			ofClamp(r, 0, 5);
			ofVec3f loc;
			loc.x = center.x + cos(rad)*r;
			loc.y = center.y + sin(rad)*r;
			loc.z = 0;
			circle.addVertex(loc);
		}
		circle.close();

		ofSetColor(newColor);
		ofBeginShape();
		for (int i=0;i<circle.getVertices().size();i++){
			 ofVertex(circle.getVertices().at(i).x, circle.getVertices().at(i).y, -1000); 
		}
		ofEndShape();

		radius -= 50;
		k -= 10;
		newColor.setBrightness(newColor.getBrightness() - 15);
	}
	ofPopStyle();
	ofPopMatrix();
	
}

void djMode::drawCircle(float radius, ofColor _color, vector<float> v){
	const float DEG2RAD = 3.14159/180;
	float centerx = ofGetWidth()/2;
	float centery = ofGetHeight()/2;
	ofPushStyle();
	//ofSetLineWidth(1);
	ofSetColor(_color);
	//ofFill();
	ofPolyline l;

	for (int i=0; i<360; i++){
		float addMe = (v[i] * 800.0f);
		if (addMe > 10) addMe = 10; 
		else if (addMe < -10) addMe = -10;
		float deg_rad = i * DEG2RAD;
		float r = radius + addMe;
		l.addVertex(centerx + cos(deg_rad)*r, centery + sin(deg_rad)*r,0);
	}
	l.close();
	ofBeginShape();
	for (int i=0;i<l.getVertices().size();i++){
		 ofVertex(l.getVertices().at(i).x, l.getVertices().at(i).y); 
	}
	ofEndShape();
	ofPopStyle();
}

void djMode::drawImage(){
	int k = 3;
	ofPushStyle();
	ofSetHexColor(0x000000);
	//printf("\n chan %d",albumArt.getPixelsRef().getNumChannels());
	unsigned char * pixels = albumArt.getPixels();
	for (int x=0; x< albumArt.width; x+= 5){
		for (int y=0; y< albumArt.height; y+= 5){
			int loc = x + y*albumArt.width;
			unsigned char r = pixels[loc];
			float val = 1-((float)r / 255.0f);
			//float test = waves(x,y,BnW_image.width,.7,1);
			//float testt = sin(val);
			ofSetColor(r,volf*50);
			float limitVol = (volf);
			if (limitVol > 4) limitVol = 4;
			ofCircle((150 + x)*k,(20+y)*k, val + limitVol );

		}
	}

	ofPopStyle();
}

void djMode::drawPointCloud() {
	ofPushMatrix();
	ofScale(1, -1, -1); 
	ofTranslate(0, 0, -1000); // center the points a bit;

	ofPushStyle();
	ofSetColor(smartColor);
	for (int i=0; i<lines.size();i++){
		ofSetLineWidth(2);
		lines[i].draw();
	}
	glPointSize(3);
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);

	ofPopStyle();
	ofPopMatrix();
	lines.clear();
	mesh.clear();
	
}



//--------------------------------------------------------------
void djMode::exit() {
	//kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void djMode::DJkeyPressed (int key) {
	switch (key) {
			
		case'k':
			bDrawPointCloud = false;			
			bDrawMeshCloud = false;			break;

		//case'p':
		//	if (bDrawPointCloud){
		//		//bDrawPointCloud=false;
		//		bcloth = true;
		//		bDrawPointCloud = false;
		//	}
		//	else{
		//		bcloth = false;
		//		bDrawPointCloud = true;
		//	}
			//bDrawPointCloud = !bDrawPointCloud;			
			//bDrawMeshCloud = !bDrawMeshCloud;			
			break;

			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;

		case '=':
			ofSaveFrame();
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
}

//--------------------------------------------------------------
void djMode::DJmouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void djMode::DJwindowResized(int w, int h)
{}

void djMode::mouseMoved(int x, int y ){
    //directional.setPosition(x, y, 100);
    
}

