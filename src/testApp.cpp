#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	/*--------GUI-----------*/
	//init color palette
	cmain.setHex(0xe6e6e6); //background grey
	ccomp1.setHex(0xd4ddd4); //highlight green
	ccomp2.setHex(0xf58d28); //accent orange
	ccomp3.setHex(0xf56494); //accent pink   
	ccomp4.set(130,255,255); //highlight blue
	ccomp5.setHex(0x5f5f5f); //dark grey

	drawDJ = false;
	drawAud = false;
	drawDisplay = true;
	
	slider1 = 80;
	slider2 = 200;

	initRects();
	guiSetup();
	ofEnableSmoothing();

}

//--------------------------------------------------------------
void testApp::update(){
	if(drawDJ){
		DJ.update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(cmain);

	if(drawDJ){
		ofPushStyle();
		ofSetColor(ccomp5);
		
		DJ.draw();
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(drawDJ){
		DJ.DJkeyPressed(key);
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

void testApp::initRects(){
	float kinectWidth = ofGetWidth()/3;
	float kinectHeight = ofGetHeight()/3 * (480.0/640.0);
	djRect = ofRectangle(0,0,kinectWidth, kinectHeight);
	audRect = ofRectangle(0,0,kinectWidth, kinectHeight);
}

void testApp::guiColors(ofxUIWidget *w){
	w->setColorBack(ccomp1);
	w->setColorFill(ccomp2);
	w->setColorFillHighlight(ccomp4);
	w->setColorOutline(ccomp2);
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
		drawDJ = true;
		DJ.setup();
	}
	else if(name == "physics mode")
	{
		drawDJ = false;
	}
	else if(name == "audience mode")
	{
		drawDJ = false;
	}
	else if(name == "physics mashup")
	{
		drawDJ = false;
	}
	/*---------------------------------*/

    if(name == "RENDER"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawDisplay = toggle->getValue();
    }
    if(name == "DJ"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawDJ= toggle->getValue();
    }
	if(name == "AUDIENCE"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawAud= toggle->getValue();
    }
    else if(name == "Style1")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		slider1 = slider->getScaledValue(); 
	}
    else if(name == "Style2")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		slider2 = slider->getScaledValue(); 
	}   
}

void testApp::guiSetup(){

    float dim = 16;
	float labelOffset = 20;
	guiWidth = 700;
	int guiHeight =300;
	ofxUIWidget *w;

	vector<string> names;
    names.push_back("physics mode");
    names.push_back("dJGod mode");
	names.push_back("video mashup");
	names.push_back("audience mode");

    //ofxUi doesn't update your variables for you, so if you add any extra toggles,
    //make sure to add the corresponding vars to the gui catch all function below.  
    gui = new ofxUICanvas(0,0,guiWidth, guiHeight);

	w = gui->addWidgetDown(new ofxUILabel("AiVDJ", OFX_UI_FONT_LARGE)); guiColors(w);
	gui->addWidgetDown(new ofxUISpacer(guiWidth - labelOffset, 2)); 
	w = gui->addWidgetDown(new ofxUIRadio("MODES", names, OFX_UI_ORIENTATION_HORIZONTAL,dim*2,dim*2,0,-100) ); guiColors(w);
    w = gui->addWidgetDown(new ofxUIToggle( "RENDER", drawDisplay, dim, dim)); guiColors(w);
    
    //Sliders for style
	w = gui->addWidgetEastOf(new ofxUISlider(300, dim, 0,255, slider1, "Style1"),"RENDER"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUISlider(300, dim, 0,255, slider2, "Style2"),"Style1"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUIToggle("DJ", drawDJ, dim, dim),"Style2"); guiColors(w);
	w = gui->addWidgetEastOf(new ofxUIToggle("AUDIENCE", drawAud, dim, dim), "DJ"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUITextInput("input", "describe your set", 250, dim*2),"AUDIENCE"); guiColors(w);

  
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
   
}
//--------------------------------------------------------------
void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");     
    delete gui; 

	DJ.exit();
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