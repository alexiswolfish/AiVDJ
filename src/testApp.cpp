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

	drawDj = false;
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

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(cmain);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

}

void testApp::guiColors(ofxUIWidget *w){
	w->setColorBack(ccomp1);
	w->setColorFill(ccomp2);
	w->setColorFillHighlight(ccomp4);
}

void testApp::guiSetup(){

    float dim = 16;
	float labelOffset = 20;
	guiWidth = 400;
	ofxUIWidget *w;

    //ofxUi doesn't update your variables for you, so if you add any extra toggles,
    //make sure to add the corresponding vars to the gui catch all function below.  
    gui = new ofxUICanvas(0,0,guiWidth, ofGetHeight());

	w = gui->addWidgetDown(new ofxUILabel("AiVDJ", OFX_UI_FONT_LARGE)); guiColors(w);
	gui->addWidgetDown(new ofxUISpacer(guiWidth - labelOffset, 2)); 
    w = gui->addWidgetDown(new ofxUIToggle( "RENDER", drawDisplay, dim, dim)); guiColors(w);
    
    //Sliders for style
	w = gui->addWidgetEastOf(new ofxUISlider(300, dim, 0,255, slider1, "Style1"),"RENDER"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUISlider(300, dim, 0,255, slider2, "Style2"),"Style1"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUIToggle("DJ", drawDj, dim, dim),"Style2"); guiColors(w);
	w = gui->addWidgetEastOf(new ofxUIToggle("AUDIENCE", drawAud, dim, dim), "DJ"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUITextInput("input", "describe your set", 250, dim*2),"AUDIENCE"); guiColors(w);
	
  
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
   
}

void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
    
    if(name == "RENDER"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawDisplay = toggle->getValue();
    }
    if(name == "DJ"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawDj= toggle->getValue();
    }
	if(name == "AUDIENCE"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawAud= toggle->getValue();
    }
   /* else if(name == "max number of features"){
        ofxUIRotarySlider *slider = (ofxUIRotarySlider *) e.widget;
        featureMax = slider->getScaledValue();
    }*/
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

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}