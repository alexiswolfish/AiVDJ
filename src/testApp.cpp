#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	//init color palette
	cmain.setHex(0xe6e6e6); //background grey
	ccomp1.setHex(0xd4ddd4); //highlight green
	ccomp2.setHex(0xf58d28); //accent orange
	ccomp3.setHex(0xf56494); //accent pink   
	ccomp4.set(130,255,255); //highlight blue
	ccomp5.setHex(0x5f5f5f); //dark grey
	white = ofColor(255,255,255);

	/*-------Sound------*/
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	soundStream.listDevices();

	// 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)

	int bufferSize = 1024;

	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	//volHistory.assign(400, 0.0);

	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;

	//soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
	ofSoundStreamSetup(0, 1, this, 44100, bufferSize, 4);

	/*--------GUI-----------*/
	drawDJKinect = false;
	drawAudKinect = false;
	drawDisplay = true;
	drawSound = true;
	mode = PHYSICS;

	DjDepthSliderLow = 0;

	DjDepthSliderHigh = 1300;
	//testItt = 40; 

	guiSetup();
	initRects();
	ofEnableSmoothing();

	/*-------Alex-------*/
	physics.setup();
	numParticles = 0;
	/*-------Jake-------*/
	DJMODE.setup();
	/*------Melissa-----*/
	Aud.setup();
}


//--------------------------------------------------------------
void testApp::update(){
	/*-------Sound------*/
	//lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
	audio->addPoint(scaledVol*100);

	/*-------kinect side displays------*/
	if(drawDJKinect){
		DJMODE.update(DjDepthSliderLow, DjDepthSliderHigh);
	}
	if(drawAudKinect){
		Aud.update();
	}

	bd.updateFFT();


	/*-------Modes-----*/
	switch(mode){
		case DJ:
			DJMODE.update(DjDepthSliderLow, DjDepthSliderHigh);
			if (!DJMODE.WheresMyDj){mode = PHYSICS;}
			break;
		case AUD:
			Aud.update();
			break;
		default:
		case PHYSICS:
			physics.addParticles(numParticles);
			//physics.updateSources(scaledVol * 190.0f);
			physics.updateSources(left[4]*180.0f);
			physics.update();
			break;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(cmain);


	//sound
	if(drawSound){
		drawVolGraphs();
		drawBeatBins();
	}

	//modes
	if(drawDisplay){
		switch(mode){
		case DJ:
			DJMODE.draw();
			break;
		case AUD:
			Aud.draw();
			break;
		case PHYSICS:{
			physics.render();
			}
			break;
		case VID:
			{
			}
			break;
		default:
			{
				ofPushStyle();
				ofSetColor(white);
				ofRect(displayRect);
				ofPopStyle();
			}
			break;
		}
	}

	if(drawDJKinect){
		ofPushMatrix();
		ofRect(djRect);
		ofTranslate(djRect.x, djRect.y);
		ofPushStyle();
		DJMODE.kinect.drawDepth(0, 0, djRect.width, djRect.height);
		DJMODE.kinect.draw(0, 0, djRect.width, djRect.height);
		ofPopStyle();
		ofPopMatrix();
	}
	if(drawAudKinect){
		ofPushMatrix();
		ofRect(audRect);
		ofTranslate(audRect.x, audRect.y);
		ofPushStyle();
		Aud.kinect.drawDepth(0, 0, audRect.width, audRect.height);
		Aud.kinect.draw(0, 0, audRect.width, audRect.height);
		ofPopStyle();
		ofPopMatrix();
	}


}
void testApp::drawBeatBins(){
	float rectWidth = 512;
	float rectHeight = 150;
	float spacer = 16;
	ofPushMatrix();
	ofTranslate(ofGetWidth()- (rectWidth+spacer),ofGetHeight()-(rectHeight*4 + spacer*4), 0);
	ofSetColor(155,155,75);

    for (int i = 1; i < (int)rectWidth; i++){
        if(i % 16 == 0)
            ofSetColor(200,0,0);
        else
            ofSetColor(155,155,75);
		ofLine(10+(i*3),150,  10+(i*3),150-bd.magnitude[i]*10.0f);
	}
	//ofTranslate(0, rectHeight + spacer, 0);
	    ofSetColor(134,113,89);
	for (int i = 1; i < (int)rectWidth/2; i++){
        if(i % 16 == 0)
            ofSetColor(200,0,0);
        else
            ofSetColor(134,113,89);
		ofLine(10+(i*3),300,  10+(i*3),300-bd.magnitude_average[i]*10.0f);
	}
	ofPopMatrix();
}

void testApp::drawVolGraphs(){
	// draw the left channel:
	float rectWidth = 512;
	float rectHeight = 150;
	float spacer = 16;

	ofPushStyle();
		ofPushMatrix();
		ofNoFill();
		ofTranslate(ofGetWidth()- (rectWidth+spacer),ofGetHeight()-(rectHeight*2 + spacer*2), 0);
			
		ofSetColor(white);
		ofDrawBitmapString("Left Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, rectWidth, rectHeight);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < left.size(); i++){
				ofVertex(i*2, 100 -left[i]*180.0f);
			}
			ofEndShape(false);

		ofTranslate(0, rectHeight + spacer, 0);
			
		ofSetColor(white);
		ofDrawBitmapString("Right Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, rectWidth, rectHeight);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < right.size(); i++){
				ofVertex(i*2, 100 -right[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(drawDJ){
		DJMODE.DJkeyPressed(key);
	} else if (drawAud) {
		Aud.AudkeyPressed(key);
	}
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	if(drawDJ){DJMODE.directional.setPosition(x, y, 100);}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	ofVec3f f = ofVec3f(0,0,50);
    
    DJMODE.controller.particles[200]->addForce(f);

	if(mode == PHYSICS)
		physics.mousePressed(physicsMode::source::ORBIT, ofVec3f((float)x,(float)y,0));

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	ofRectangle guiRect = ofRectangle(0,0,guiWidth, guiHeight);
	if(!guiRect.inside(ofVec2f(x,y)))
		physics.mousePressed( sourceType, ofVec3f(x,y,0));
}

void testApp::audioReceived(float* input, int bufferSize, int nChannels) {

    bd.audioReceived(input, bufferSize);

}

void testApp::audioIn(float * input, int bufferSize, int nChannels){	
	// bd.audioReceived(input, bufferSize);

	float curVol = 0.0;
	int numCounted = 0;	
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;

	}
	//mean rms
	curVol /= (float)numCounted;
	//root rms 
	curVol = sqrt( curVol );
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	bufferCounter++;
	
	/*------Beat Detection-------*/
	bd.audioReceived(input, bufferSize);
}

void testApp::initRects(){
	float spacer = 16;

	//vertical
	float kinectHeight = (ofGetHeight() - guiHeight)/2 - spacer*3;
	float kinectWidth = kinectHeight*(640.0/480.0);
	djRect = ofRectangle(spacer, guiHeight+spacer, kinectWidth, kinectHeight);
	audRect = ofRectangle(spacer, djRect.getMaxY() + spacer, kinectWidth, kinectHeight);
}

void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
    
	
	/*---------------------------------*
	These functions control the 4 big buttons up top.
	Update all of the booleans and make sure to call
	your setup functions below
	*---------------------------------*/
	if(name == "dJGod mode")
	{
		if (!DJMODE.WheresMyDj){DJMODE.WheresMyDj = true;}
		mode = DJ;
	}
	else if(name == "physics mode")
	{
		mode = PHYSICS;
	}
	else if(name == "audience mode")
	{
		mode = AUD;
	}
	else if(name == "video mashup")
	{
		mode = VID;
	}
	/*---------------------------------*/

    if(name == "RENDER"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawDisplay = toggle->getValue();
    }
    if(name == "DJ"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawDJKinect = toggle->getValue();
    }
	if(name == "AUDIENCE"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawAudKinect = toggle->getValue();
    }
    else if(name == "dJ depth threshold")
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget; 
		DjDepthSliderHigh = slider->getScaledValueHigh(); 
		DjDepthSliderLow = slider->getScaledValueLow(); 
	}
 //   else if(name == "dJ testt")
	//{
	//	ofxUISlider *slider = (ofxUISlider *) e.widget; 
	//	testItt = slider->getScaledValue(); 
	//}
    else if(name == "aud depth threshold")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		slider2 = slider->getScaledValue(); 
	}     
	else if(name == "beat debug"){
		 ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
		 drawSound = toggle->getValue();
	}
	/*----Particle Sliders-----*/
	else if (name == "particle rebirth")
	{
		ofxUIRotarySlider *r = (ofxUIRotarySlider *)e.widget;
		numParticles = r->getScaledValue();
	}
	else if (name == "emit")
		sourceType = physicsMode::source::EMIT;
	else if (name == "sink")
		sourceType = physicsMode::source::SINK;
	else if (name == "orbit")
		sourceType = physicsMode::source::ORBIT;
}
void testApp::guiColors(ofxUIWidget *w){
	/*w->setColorBack(ccomp1);
	w->setColorFill(ccomp2);
	w->setColorFillHighlight(ccomp4);
	w->setColorOutline(ccomp2);*/
}
void testApp::guiSetup(){

    float dim = 16;
	float labelOffset = 20;
	guiWidth = 600;
	guiHeight = 400;
	ofxUIWidget *w;

	vector<string> names;
    names.push_back("physics mode");
    names.push_back("dJGod mode");
	names.push_back("video mashup");
	names.push_back("audience mode");

	vector<string> particleModes;
	particleModes.push_back("emit");
	particleModes.push_back("sink");
	particleModes.push_back("orbit");

	int buffersize = 400;
	for(int i=0; i<buffersize; i++)
		volHistory.push_back(0);

    //ofxUi doesn't update your variables for you, so if you add any extra toggles,
    //make sure to add the corresponding vars to the gui catch all function below.  
    gui = new ofxUICanvas(0,0,guiWidth, guiHeight);
	gui->setTheme(OFX_UI_THEME_ZOOLANDER);

	w = gui->addWidgetDown(new ofxUILabel("AiVDJ", OFX_UI_FONT_LARGE)); guiColors(w);
	w = gui->addWidgetDown(new ofxUISpacer(guiWidth - labelOffset, 2)); w->setColorFill(white);
	w = gui->addWidgetDown(new ofxUIRadio("MODES", names, OFX_UI_ORIENTATION_HORIZONTAL,dim*2,dim*2,0,-100) );guiColors(w); 
    w = gui->addWidgetDown(new ofxUIToggle( "RENDER", drawDisplay, dim, dim));guiColors(w);
    
    //Sliders for style
	w = gui->addWidgetEastOf(new ofxUIRangeSlider("dJ depth threshold", 0, 5000, 0, 1300, dim*25, dim),"RENDER"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUIRangeSlider("aud depth threshold", 0, 5000, 440, 4000, dim*25, dim),"dJ depth threshold"); guiColors(w);
	//w = gui->addWidgetSouthOf(new ofxUISlider("dJ testt", 1, 100, 40, dim*25, dim),"aud depth threshold"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUIToggle("DJ", drawDJKinect, dim, dim),"aud depth threshold"); guiColors(w);
	w = gui->addWidgetEastOf(new ofxUIToggle("AUDIENCE", drawAudKinect, dim, dim), "DJ"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUITextInput("input", "describe your set", dim*12, dim*2),"AUDIENCE");guiColors(w);
	w = gui->addWidgetEastOf(new ofxUIRotarySlider(dim*8, 0, 200, numParticles, "particle rebirth"),"input");guiColors(w);
	w = gui->addWidgetEastOf(new ofxUIRadio("source", particleModes, OFX_UI_ORIENTATION_VERTICAL,dim,dim,0,0),"particle rebirth" );guiColors(w); 
    audio = (ofxUIMovingGraph *) gui->addWidgetSouthOf(new ofxUIMovingGraph(dim*12, 64, volHistory, buffersize, -100, 100, "Volume"),"input"); 
	w = gui->addWidgetSouthOf(new ofxUIToggle("beat debug", drawSound, dim, dim),"Volume");guiColors(w);
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}
//--------------------------------------------------------------
void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");     
    delete gui; 

	DJMODE.exit();
	Aud.exit();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}