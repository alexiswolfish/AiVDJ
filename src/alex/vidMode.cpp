#include "vidMode.h"

 
  /*----------------------------------
	Particle Class
  *-----------------------------------*/
  vidMode::ParticleController::Particle::Particle()
  {
    pos = ofVec3f(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()));
    vel = ofVec3f(0,0,0);
  }
    
  void vidMode::ParticleController::Particle::update(int x, int y)
  {
    vel.x = y/1.5*(ofNoise(x*100/.1+pos.y/100)-0.5);
    vel.y = x/1.5*(ofNoise(y*2/.1+pos.x/100)-0.5);
	vel.limit(ofRandom(30));
    pos = pos+(vel);
      
    if(pos.x<10)pos.x+=ofGetWidth();
    if(pos.x>ofGetWidth())pos.x-=ofGetWidth();
    if(pos.y<120)pos.y+=ofGetHeight();
    if(pos.y>ofGetHeight())pos.y-=ofGetHeight();
  }
  
  void vidMode::ParticleController::Particle::render(int x, int y)
  {
	ofPushStyle();
	ofNoFill();
    ofSetColor(x/1.5,y/1.5,ofRandom(0,255),ofRandom(2,40));
	//ofSetColor(x/1.5,ofRandom(2,40));

	//RGB
	int numPix = 480*640*3;
	int realCoord = (640*3)*y + x;

	int pixOffsetX = ((ofGetWidth()/2)-320);
	int pixOffsetY = ((ofGetHeight()/2)-240);


	//ofSetColor(x/1.5,y/1.5,ofRandom(0,255));
	ofSetLineWidth(3);	
    ofLine(pos.x,pos.y,pos.x-vel.x,pos.y-vel.y);
	ofPopStyle();
  }


  /*----------------------------------
	ParticleController Class
  *-----------------------------------*/
  vidMode::ParticleController::ParticleController(){
  numParticles = 9000;
      for(int i = 0; i < numParticles; i++)
    {
		Particle p;
		particles.push_back(p);
    }
  }

  vidMode::ParticleController::ParticleController(int p)
  {
	numParticles = p;
    for(int i = 0; i < numParticles; i++)
    {
		Particle p;
		particles.push_back(p);
    }
  }
    
  void vidMode::ParticleController::update(int x, int y)
  {
    for(int i = 0; i < numParticles; i++)
    {
      particles[i].update(x,y);
    }
  }
    
  void vidMode::ParticleController::render(int x, int y)
  {
    for(int i = 0; i < numParticles; i++)
    {
      particles[i].render(x,y);
    }
  }

/*----------------------------------
	vidMode
 *-----------------------------------*/
 vidMode::vidMode(){}
 vidMode::~vidMode(){}
void vidMode::setup()
{
	/*---------------Flags--------------*/
	ofEnableAlphaBlending();

	drawParticles = true;
	ofSetBackgroundAuto(false);

	seedX = ofRandom(100, 1900);
	seedY = ofRandom(100, 1000);

	/*---------------Video----------------*/
	curVid.loadMovie("vid/blockhead.mp4");
	curVid.setFrame(800);
	curVid.setVolume(0);
	curVid.play();

	vidWidth = curVid.width * (float)((float)ofGetWidth()/curVid.width);
	vidHeight = curVid.height * (float)((float)ofGetHeight()/curVid.height);

	bpm = 0;
	numParticles = 7000;
	p = ParticleController(numParticles);

	/*-----------Shaaaaders ftw---------------*/
	maskShader.load("composite.vert", "composite.frag");
	topLayer.loadImage("bg.png");
	particleTexture.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	maskFbo.allocate(ofGetWidth(), ofGetHeight());
	fbo.allocate(ofGetWidth(), ofGetHeight());

	//hurts my heart, but what can you do
	string shaderProgram = "#version 120\n \
    #extension GL_ARB_texture_rectangle : enable\n \
    \
    uniform sampler2DRect tex0;\
    uniform sampler2DRect maskTex;\
    \
    void main (void){\
    vec2 pos = gl_TexCoord[0].st;\
    \
    vec3 src = texture2DRect(tex0, pos).rgb;\
    float mask = texture2DRect(maskTex, pos).r;\
    \
    gl_FragColor = vec4( src , mask);\
    }";
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
    shader.linkProgram(); 

	//clear FBOs
	maskFbo.begin();
    ofClear(0,0,0,255);
    maskFbo.end();
    
    fbo.begin();
    ofClear(0,0,0,255);
    fbo.end();


}
void vidMode::update(int x, int y, float _bpm, beatDetect bd)
{
	//draw into an ofImage
	bpm = _bpm;
	if(!drawParticles)
		updateSpeed();
	else
		p.update(seedX, seedY);

	curVid.update();

	//fill fbos
	maskFbo.begin();
	 p.render(seedX, seedY);
	 ofSetColor(0,0,0,ofRandom(1,20));
	 ofRect(0,0,ofGetWidth(), ofGetHeight());

	 ofSetColor(255,255,255,ofRandom(0,18));
	 ofRect(0,0,ofGetWidth(), ofGetHeight());
    maskFbo.end();
 
	fbo.begin();
    // Cleaning everthing with alpha mask on 0 in order to make it transparent for default
    ofClear(0, 0, 0, 0); 
    
    shader.begin();
    shader.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1 );

	curVid.draw(0,0,vidWidth, vidHeight);

    shader.end();
    fbo.end();

	if(bd.isSnare() && bd.isKick() && bd.isSnare()){// && (ofGetElapsedTimef()-time > 3))){
		time = ofGetElapsedTimef();
		float clear = ofMap(bpm, 60, 800, 1, 45);
		//clear buffer
		maskFbo.begin();
		ofClear(0,0,0,clear);
		maskFbo.end();
		//refresh x and y
		
		seedX = ofRandom(100, 1900);
		seedY = ofRandom(100, 1000);
	}
}

void vidMode::draw(int x, int y)
{
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	ofSetHexColor(0xFFFFFF);
	//draw video only
	if(!drawParticles){
		curVid.draw( 0, 0, vidWidth, vidHeight);
		ofDrawBitmapString("vid speed: " + ofToString(curVid.getSpeed(),2),0,400);
	}
	//draw particle overlay
	else{
		ofSetColor(255,255);
		topLayer.draw( 0, 0, vidWidth, vidHeight);
		fbo.draw(0,0);

	}
}

void vidMode::keyPressed(int key){
	if(key == '0'){
		curVid.firstFrame();
	}
	if(key == '1'){
		drawParticles = !drawParticles;
		ofSetBackgroundAuto(!drawParticles);
	}
	if(key == '2'){
		maskFbo.begin();
		ofClear(0,0,0,30);
		maskFbo.end();

		seedX = ofRandom(100, 1900);
		seedY = ofRandom(100, 1000);
	}
	if(key == '3'){
		seedX = ofRandom(100, 1900);
		seedY = ofRandom(100, 1000);
	}
	if(key == 'w')
		ofSaveFrame();
}

void vidMode::updateSpeed(){
	
	float speed = ofMap(bpm, 60, 600, 0, 5);
	curVid.setSpeed(speed);
}