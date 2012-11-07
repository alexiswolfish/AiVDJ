#include "physicsMode.h"

physicsMode::physicsMode(){
	birthRate = 50;
	maxParticles = 1000;
}

void physicsMode::mousePressed(source::Type t, ofVec3f pos){
	sources.push_back(source(pos, t));
}

void physicsMode::setup(){
	ofEnableAlphaBlending();
	particleImg.loadImage("spark.png");

	updatePos.load("","shaders/posUpdate.frag");// shader for updating the texture that store the particles position on RG channels
    updateVel.load("","shaders/velUpdate.frag");// shader for updating the texture that store the particles velocity on RG channels
    // Frag, Vert and Geo shaders for the rendering process of the spark image
    updateRender.setGeometryInputType(GL_POINTS);
	updateRender.setGeometryOutputType(GL_TRIANGLE_STRIP);
	updateRender.setGeometryOutputCount(6);
    updateRender.load("shaders/render.vert","shaders/render.frag","shaders/render.geom");
    // Seting the textures where the information ( position and velocity ) will be

	//adding sources
	sources.push_back(source(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 0), source::ORBIT));
	sources.push_back(source(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 0), source::SINK));
	addParticles(1000);
}

void physicsMode::addParticles(int amt){
	if(particles.size() < maxParticles){
	for(int i=0; i<amt; i++)
		particles.push_back(particle());}
}

void physicsMode::update(){
	for(vector<source>::iterator e = sources.begin(); e != sources.end(); ++e){
		for(vector<particle>::iterator p = particles.begin(); p != particles.end(); ++p){
			p->applyForce(*e, e->mass*10);
			p->update();
		}
	}
	for(vector<particle>::iterator p = particles.begin(); p != particles.end();){
		if(p->isDead)
			p = particles.erase(p);
		else
			p++;
	}
}

void physicsMode::render(){
	for(vector<source>::iterator e = sources.begin(); e != sources.end(); e++){
		e->render();
	}
	for(vector<particle>::iterator p = particles.begin(); p != particles.end(); p++)
		p->render();
}

void physicsMode::updateSources(float vol){
	for(vector<source>::iterator e = sources.begin(); e != sources.end(); ++e){
		e->radius = vol;
		e->mass = vol;
	}
}

/*--------------------------------*
	Source Class
 *--------------------------------*/

physicsMode::source::source(ofVec3f initPos, Type _type){
	loc = initPos;
	radius = 15;
	mass = 50;
	type = _type;
	life = 1000;
}

void physicsMode::source::update(){
}

void physicsMode::source::render(){
	ofPushStyle();
	if(type == EMIT)
		ofSetColor(0,255,255);
	else
		ofSetColor(255,0,255);
	ofCircle(loc.x, loc.y, radius);
	ofPopStyle();
}

/*--------------------------------*
	Particle Class
 *--------------------------------*/
physicsMode::particle::particle(){
	loc = ofVec3f(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0);
	mass = ofRandom(1,5);
	maxSpeed = 200;
	magnitude = 0;
	angle = 0;
	death = 0.96;
	age = 0;
	lifespan = 1000;
	isDead = false;
	vel = ofVec3f(ofRandom(-15,15),ofRandom(-15,15),0);
	acc = ofVec3f(ofRandom(-15,15),ofRandom(-15,15),0);
}
physicsMode::particle::~particle(){
}
physicsMode::particle::particle(ofVec3f _loc, float m, int life){
	loc = _loc;
	mass = m;
	maxSpeed = 200;
	magnitude = 0;
	angle = 0;
	death = 0.9;
	age = 0;
	lifespan = life;
	isDead = false;

	vel = ofVec3f(ofRandom(-15,15),ofRandom(-15,15),0);
	acc = ofVec3f(ofRandom(-15,15),ofRandom(-15,15),0);
}


void physicsMode::particle::update(){
	pLoc = loc;
	vel = vel+acc;
	vel.limit(maxSpeed);
	loc = loc + acc;
	acc = acc * death;
	age++;
	if(age > lifespan)
		isDead = true;
}


void physicsMode::particle::applyForce(source a, float range){
	ofVec3f m = ofVec3f(0,0,0);
    ofVec3f dirToPull = ofVec3f(loc.x, loc.y, 0);
    dirToPull = dirToPull - a.loc;
    float distToPull = dirToPull.length();

	if(distToPull < range){
		float deathThresh = (range - distToPull) / range;
		deathThresh = deathThresh * deathThresh;
		if(deathThresh > 0.95){
			isDead = true; //possibly respawn?
		}
		if((a.type == source::EMIT) || a.type == source::SINK){
			//straight up attraction/repuslion forces
			float theta, F;
			F = mass * a.mass;
			m.x = (mass*loc.x + a.mass*a.loc.x)/(mass+a.mass);
			m.y = (mass*loc.y + a.mass*a.loc.y)/(mass+a.mass);

			if(a.type == source::EMIT)
				theta = findAngle( loc.x - m.x, loc.y -m.y);
			else if(a.type == source::SINK)
				 theta = findAngle( m.x - loc.x, m.y - loc.y );
    
			m.x = (F*cos(theta)) / distToPull;
			m.y = (F*sin(theta)) / distToPull;
    
			angle = findAngle(m.x, m.y);
			acc.x += (m.length() * cos(angle));
			acc.y += (m.length() * sin(angle)); 
		}
		else{
			//orbit
			float F = mass * a.mass;
			dirToPull = dirToPull.normalize();
			ofVec3f tanForce = ofVec3f(dirToPull.y, - dirToPull.x, 0);
			tanForce = tanForce * (deathThresh*10);
			acc = (acc + tanForce);
		}
	}
}


float physicsMode::particle::findAngle(float x, float y){
  float theta;
  if(x == 0) {
    if(y > 0) {
      theta = HALF_PI;
    }
    else if(y < 0) {
      theta = 3*HALF_PI;
    }
    else {
      theta = 0;
    }
  }
  else {
    theta = atan( y / x );
    if(( x < 0 ) && ( y >= 0 )) { theta += PI; }
    if(( x < 0 ) && ( y < 0 )) { theta -= PI; }
  }
  return theta;
}

void physicsMode::particle::render(){
	ofPushStyle();
	ofSetColor(255,0,255);
	ofEllipse(loc.x, loc.y, 2,2);
	ofLine(pLoc, loc);
	ofPopStyle();
}