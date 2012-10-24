#include "physicsMode.h"

physicsMode::physicsMode(){
	birthRate = 50;
}

void physicsMode::mousePressed(source::Type t, ofVec3f pos){
	sources.push_back(source(pos, t));
}

void physicsMode::setup(){
	sources.push_back(source(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 0), source::ORBIT));
	addParticles(1000);
	printf("%d", particles.size());
}

void physicsMode::addParticles(int amt){
	for(int i=0; i<amt; i++)
		particles.push_back(particle());
}

void physicsMode::update(){
	for(vector<source>::iterator e = sources.begin(); e != sources.end(); ++e){
		for(vector<particle>::iterator p = particles.begin(); p != particles.end(); ++p){
			p->applyForce(*e,400);
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
		ofSetColor(255,0,20);
	ofEllipse(loc.x, loc.y, 10, 10);
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
	death = 0.8;
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