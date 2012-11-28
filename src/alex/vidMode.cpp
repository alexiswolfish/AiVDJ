#include "vidMode.h"

 
  /*----------------------------------
	Particle Class
  *-----------------------------------*/
  vidMode::ParticleController::Particle::Particle()
  {
    pos = ofVec3f(ofRandom(ofGetScreenWidth()),ofRandom(ofGetScreenHeight()));
    vel = ofVec3f(0,0,0);
  }
    
  void vidMode::ParticleController::Particle::update(int x, int y)
  {
    vel.x = y/1.5*(ofNoise(x*100/.1+pos.y/100)-0.5);
    vel.y = x/1.5*(ofNoise(y*2/.1+pos.x/100)-0.5);
	vel.limit(ofRandom(30));
    pos = pos+(vel);
      
    if(pos.x<10)pos.x+=ofGetScreenWidth();
    if(pos.x>ofGetScreenWidth())pos.x-=ofGetScreenWidth();
    if(pos.y<120)pos.y+=ofGetScreenHeight();
    if(pos.y>ofGetScreenHeight())pos.y-=ofGetScreenHeight();
  }
  
  void vidMode::ParticleController::Particle::render(int x, int y, unsigned char * p)
  {
	ofPushStyle();
	ofNoFill();
   // ofSetColor(x/1.5,y/1.5,ofRandom(0,255),ofRandom(2,40));

	//RGB
	int numPix = 480*640*3;
	int realCoord = (640*3)*y + x;

	int pixOffsetX = ((ofGetWindowWidth()/2)-320);
	int pixOffsetY = ((ofGetWindowHeight()/2)-240);


	ofSetColor(x/1.5,y/1.5,ofRandom(0,255));
	ofSetLineWidth(1.5);	
    ofLine(pos.x,pos.y,pos.x-vel.x,pos.y-vel.y);
	ofPopStyle();
  }


  /*----------------------------------
	ParticleController Class
  *-----------------------------------*/
  vidMode::ParticleController::ParticleController(){
  numParticles = 7000;
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
    
  void vidMode::ParticleController::render(int x, int y, unsigned char * p)
  {
    for(int i = 0; i < numParticles; i++)
    {
      particles[i].render(x,y, p);
    }
  }

/*----------------------------------
	vidMode
 *-----------------------------------*/
 vidMode::vidMode(){}
 vidMode::~vidMode(){}
void vidMode::setup()
{
  numParticles = 7000;
  p = ParticleController(numParticles);

  	camWidth 		= 640;	// try to grab at this size. 
	camHeight 		= 480;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);

}
void vidMode::update(int x, int y)
{
  p.update(x,y);

  //camera garbage
  vidGrabber.update();
	//end camera garbage
}

void vidMode::draw(int x, int y)
{
  p.render(x,y, vidGrabber.getPixels());
  ofSetHexColor(0xffffff);
  vidGrabber.draw(20,20);
  //camera garbage
 

}
