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
	white = ofColor(255,255,255);

	drawDJKinect = false;
	drawAudKinect = false;
	drawDisplay = true;
	mode = PHYSICS;
	
	slider1 = 80;
	slider2 = 200;

	guiSetup();
	initRects();
	ofEnableSmoothing();

	/*-------Jake-------*/
//	DJ.setup();

}

//--------------------------------------------------------------
void testApp::update(){
	if(drawDJ){
//		DJ.update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(cmain);

	if(drawDisplay){
		switch(mode){
		case DJ:
			{
			}
			break;
		case AUD:
			{
			}
			break;
		case PHYSICS:
			{
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
		ofPushStyle();
		ofSetColor(white);
		ofRect(djRect);
		ofPopStyle();
	}
	if(drawAudKinect){
		ofPushStyle();
		ofSetColor(white);
		ofRect(audRect);
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(drawDJ){
//		DJ.DJkeyPressed(key);
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
	float spacer = 16;

	//vertical
	float kinectHeight = (ofGetHeight() - 300)/2 - spacer*3;
	float kinectWidth = kinectHeight*(640.0/480.0);
	djRect = ofRectangle(spacer, 300+spacer, kinectWidth, kinectHeight);
	audRect = ofRectangle(spacer, djRect.getMaxY() + spacer, kinectWidth, kinectHeight);

	//horizontal

	//float kinectWidth = (ofGetWidth() - guiWidth)/2 - spacer*3;
	//float kinectHeight = kinectWidth * (480.0/640.0);
	//djRect = ofRectangle(guiWidth + spacer, spacer, kinectWidth, kinectHeight);
	//audRect = ofRectangle(djRect.getMaxX() + spacer, spacer,kinectWidth, kinectHeight);

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
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		slider1 = slider->getScaledValue(); 
	}
    else if(name == "aud depth threshold")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		slider2 = slider->getScaledValue(); 
	}   
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
	gui->setTheme(OFX_UI_THEME_ZOOLANDER);

	w = gui->addWidgetDown(new ofxUILabel("AiVDJ", OFX_UI_FONT_LARGE)); guiColors(w);
	w = gui->addWidgetDown(new ofxUISpacer(guiWidth - labelOffset, 2)); w->setColorFill(white);
	w = gui->addWidgetDown(new ofxUIRadio("MODES", names, OFX_UI_ORIENTATION_HORIZONTAL,dim*2,dim*2,0,-100) );guiColors(w); 
    w = gui->addWidgetDown(new ofxUIToggle( "RENDER", drawDisplay, dim, dim));guiColors(w);
    
    //Sliders for style
	w = gui->addWidgetEastOf(new ofxUIRangeSlider("dJ depth threshold", 0, 5000, 440, 1400, dim*25, dim),"RENDER"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUIRangeSlider("aud depth threshold", 0, 5000, 440, 4000, dim*25, dim),"dJ depth threshold"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUIToggle("DJ", drawDJKinect, dim, dim),"aud depth threshold"); guiColors(w);
	w = gui->addWidgetEastOf(new ofxUIToggle("AUDIENCE", drawAudKinect, dim, dim), "DJ"); guiColors(w);
	w = gui->addWidgetSouthOf(new ofxUITextInput("input", "describe your set", dim*12, dim*2),"AUDIENCE");guiColors(w);

  
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
   
}
//--------------------------------------------------------------
void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");     
    delete gui; 

	//DJ.exit();
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