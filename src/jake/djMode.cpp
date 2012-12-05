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
	bcloth = false;
	tiltDegr = 15;

	//try running the hand detect with the example code
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 255;
	farThreshold = 70;

	//// enable depth->video image calibration
	kinect.setRegistration(true);

	kinect.init();
	kinect.init(true); // shows infrared instead of RGB video image
	kinect.init(false, false); // disable video image (faster fps)

	kinect.open();
	////kinect.open("B00363262039047B");	// open a kinect using it's unique serial #
	//ofBackground(100, 100, 100);
	ofSetFrameRate(60);


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

	angle = 10;
	kinect.setCameraTiltAngle(angle);
	//printf("serial:'%s'", kinect.getSerial());
	//easyCam.tilt(15);
}

//--------------------------------------------------------------
void djMode::update(vector<float> &vol, float depthLow, float depthHigh, bool beat) {

	kinect.update();

	Zlow = depthLow;
	Zhigh = depthHigh;
	newBeat = beat;

	if(kinect.isFrameNew()) {

		//grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		//grayThreshNear = grayImage;
		//grayThreshFar = grayImage;
		//grayThreshNear.threshold(nearThreshold, true);
		//grayThreshFar.threshold(farThreshold);
		//cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		//
		//grayImage.flagImageChanged();
		//contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);

		//fingers = getFingerTips(grayImage);

		if (bcloth) clothShit();
		else if (bDrawPointCloud){
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
			if (beat){
				mesh.setMode(OF_PRIMITIVE_POINTS);
				int rand1 = ofRandom(128.0, 255.0);
				for(int y = 0; y < h; y += step*2) {
					for(int x = 0; x < w; x += step*2) {
						if(kinect.getDistanceAt(x, y) > 0) {
							if (kinect.getWorldCoordinateAt(x, y).z > Zhigh){	
								float sizeD = (vol[x] * 2000.0f);
								if (sizeD > 5) sizeD = 5; 
								else if (sizeD < -5) sizeD = -5;
								mesh.addColor(ofColor(rand1, 0, (kinect.getWorldCoordinateAt(x, y).z*-1) / (Zhigh/255) + 128));
								mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
								//bg_points.point.push_back(kinect.getWorldCoordinateAt(x, y));
								//bg_points.color.push_back(ofColor(rand1, 0, (kinect.getWorldCoordinateAt(x, y).z*-1) / (Zhigh/255) + 128));
								//bg_points.size.push_back(sizeD);
							}
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

}

//--------------------------------------------------------------
void djMode::updateGlobals(ofColor c, bool changeColor, float volume) {
	if (changeColor)smartColor = c;
	vol = volume;
}

//--------------------------------------------------------------


void djMode::draw() {
	ofBackground(95, 100);
	
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();

		//grayImage.draw(10, 320, 400, 300);
		//contourFinder.draw(10, 320, 400, 300);
				
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
	fingers.clear();
}



void djMode::drawPointCloud() {
	ofPushMatrix();
	ofScale(1, -1, -1); 
	ofTranslate(0, 0, -1000); // center the points a bit;

	//ofBackground(95, 100);
	ofPushStyle();
	ofSetColor(smartColor);
	for (int i=0; i<lines.size();i++){
		ofSetLineWidth(2);
	lines[i].draw();
		//int subLines;
		//vector<ofPoint> ps = lines[i].getVertices();
		//subLines = lines[i].size()/4;
		//for (int k=0; k<subLines; k++){
		//	ps.
		//}
	}
	//int temp = (int)vol;
	glPointSize(3);
	//printf("\n vol: %f", vol);
	glEnable(GL_DEPTH_TEST);
	mesh.drawVertices();
	glDisable(GL_DEPTH_TEST);
	//for (int j=0; j<bg_points.point.size(); j++){
	//	ofSetColor(bg_points.color[j]);
	//	ofSphere(bg_points.point[j].x, bg_points.point[j].y, bg_points.point[j].z, bg_points.size[j]);
	//}

	ofPopStyle();
	ofPopMatrix();
	lines.clear();
	//bg_points.color.clear();
	//bg_points.point.clear();
	//bg_points.size.clear();
	mesh.clear();
	

	//ofPushMatrix();
	//ofTranslate(10, 400, -1);
	//ofScale(0.6, 0.6, 0);
	//for(int i = 0; i < fingers.size(); i++){
	//	ofSetColor(255,255,0);
	//	//ofSetLineWidth(3);
	//	ofCircle(fingers[i].x, fingers[i].y,5);
	//	//ofLine(palmCenter.x, palmCenter.y, fingers[i].x, fingers[i].y);
	//	//ofSetLineWidth(0);
	//}
	//ofPopMatrix();
}

 

vector<ofPoint> djMode::getFingerTips( ofxCvGrayscaleImage input) {
	CvMemStorage*	storage = cvCreateMemStorage(0);
	CvSeq*			contours;
	CvPoint*		PointArray;
	
	int* hull;
	int hullsize;
	
	vector<ofPoint> fingerTips;
	
	//START TO FIND THE HULL POINTS
	cvFindContours( input.getCvImage(), storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
	
	// If there is a contour it´ll make it more simple
	if (contours)
		contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 20, 1 );
	
	
	int i = 0;
	int area = 0;
	int selected = -1;
	
	CvSeq* first_contour = contours; // Remember the first contour address
	for( ; contours != 0; contours = contours->h_next ){	// Search for the bigger countour
		CvRect rect;
		int count = contours->total;
		rect = cvContourBoundingRect(contours, 1);
		if( (rect.width*rect.height) > area ){
			selected = i;
			area = rect.width*rect.height;
		}
		i++;
	}
	
	contours = first_contour;		// Go again to the first contour
	int k = 0; 
	for( ; contours != 0; contours = contours->h_next ){		
		int i; // Indicator of cycles.
		int count = contours->total; // This is number point in contour
		CvPoint center;
		CvSize size;
		CvRect rect;
		
		rect = cvContourBoundingRect( contours, 1);
		
		if ( (k==selected) ){		// Analize the bigger contour
			palmCenter.x = rect.x + rect.width/2;
			palmCenter.y = rect.y + rect.height/2;
			
			fingerTips.clear();
			
			PointArray = (CvPoint*)malloc( count*sizeof(CvPoint) ); // Alloc memory for contour point set.
			hull = (int*)malloc(sizeof(int)*count);	// Alloc memory for indices of convex hull vertices.
			
			cvCvtSeqToArray(contours, PointArray, CV_WHOLE_SEQ); // Get contour point set.
			
			// Find convex hull for curent contour.
			cvConvexHull( PointArray,
						count,
						 NULL,
						 CV_COUNTER_CLOCKWISE,
						 hull,
						 &hullsize);
			
			int upper = 640, lower = 0;
			for	(int j=0; j<hullsize; j++) {
				int idx = hull[j]; // corner index
				if (PointArray[idx].y < upper) upper = PointArray[idx].y;
				if (PointArray[idx].y > lower) lower = PointArray[idx].y;
			}
			float cutoff = lower - (lower - upper) * 0.1f;
			
			// find interior angles of hull corners
			for (int j=0; j<hullsize; j++) {
				int idx = hull[j]; // corner index
				int pdx = idx == 0 ? count - 1 : idx - 1; //  predecessor of idx
				int sdx = idx == count - 1 ? 0 : idx + 1; // successor of idx
				
				cv::Point v1 = cv::Point(PointArray[sdx].x - PointArray[idx].x, PointArray[sdx].y - PointArray[idx].y);
				cv::Point v2 = cv::Point(PointArray[pdx].x - PointArray[idx].x, PointArray[pdx].y - PointArray[idx].y);
				
				float angle = acos( (v1.x*v2.x + v1.y*v2.y) / (norm(v1) * norm(v2)) );
				
				// low interior angle + within upper 90% of region -> we got a finger
				if (angle < 1 && PointArray[idx].y < cutoff) {
					int u = PointArray[idx].x;
					int v = PointArray[idx].y;
					
					fingerTips.push_back(ofPoint(u,v));
				}
			}
			
			// Free memory.
			free(PointArray);
			free(hull);
			
		}
		k++;
	}
	
	
	cvClearMemStorage( storage );
	//if (seqhull)
		//cvClearSeq(seqhull);
	
	
	return fingerTips;
}


void djMode::drawMeshCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	//float maxY = 0;
	float minX = 0;
	float maxX = 0;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 4; 
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
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void djMode::DJkeyPressed (int key) {
	switch (key) {
			
		case'k':
			bDrawPointCloud = false;			
			bDrawMeshCloud = false;			break;

		case'p':
			if (bDrawPointCloud){
				//bDrawPointCloud=false;
				bcloth = true;
				bDrawPointCloud = false;
			}
			else{
				bcloth = false;
				bDrawPointCloud = true;
			}
			//bDrawPointCloud = !bDrawPointCloud;			
			//bDrawMeshCloud = !bDrawMeshCloud;			
			break;
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

