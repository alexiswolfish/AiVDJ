#include "physicsMode.h"

physicsMode::physicsMode(){
	birthRate = 100;
}

void physicsMode::setup(){
	emitters.push_back(source(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 0), source::EMIT));
	sinks.push_back(source(ofVec3f(ofGetWidth()/2+200, ofGetHeight()/2+400, 0), source::SINK));
}

void physicsMode::update(){
	for(int i=0; i<emitters.size(); i++){
		emitters[i].addParticles(birthRate);
		for(int p=0; p<emitters[i].particles.size(); p++){
			for(int j=0; j<sinks.size(); j++){
				emitters[i].applyForce(emitters[i].particles[p], sinks[j]);
			}
			emitters[i].particles[p].update();
		}
	}
}

void physicsMode::render(){
	for(int i=0; i<emitters.size(); i++){
		emitters[i].render();
		for(int p=0; p<emitters[i].particles.size(); p++){
			emitters[i].particles[p].render();
		}
	}
}

/*--------------------------------*
	Source Class
 *--------------------------------*/

physicsMode::source::source(ofVec3f initPos, Type _type){
	loc = initPos;
	radius = 15;
	mass = 1;
	type = _type;
	life = 1000;
}
void physicsMode::source::addParticles(int amt){
	for(int i=0; i<amt; i++){
		particles.push_back( particle(loc, mass, life));
	}
}
void physicsMode::source::update(){
	if(type == EMIT){
		for(std::vector<particle>::iterator p = particles.begin(); p != particles.end(); ){
			if(p->isDead){
				p = particles.erase(p);
			}
			else{
				p->update();
				++p;
			}
		}
	}
}

void physicsMode::source::render(){
	ofEllipse(loc.x, loc.y, 10, 10);
}

/*--------------------------------*
	Particle Class
 *--------------------------------*/
physicsMode::particle::particle(){
	loc = ofVec3f(0,0,0);
	mass = 1;
	maxSpeed = 30;
	magnitude = 0;
	angle = 0;
	death = 0.1;
	age = 0;
	lifespan = 1000;
	isDead = false;
}
physicsMode::particle::~particle(){
}
physicsMode::particle::particle(ofVec3f _loc, float m, int life){
	loc = _loc;
	mass =m;
	maxSpeed = 30;
	magnitude = 0;
	angle = 0;
	death = 0.1;
	age = 0;
	lifespan = life;
	isDead = false;

	vel = ofVec3f(0,0,0);
	acc = ofVec3f(0,0,0);
}


void physicsMode::particle::update(){
	vel = vel+acc;
	vel.limit(maxSpeed);
	loc = loc + acc;
	acc = acc * death;

	age++;
	if(age > lifespan)
		isDead = true;
}

void physicsMode::source::applyForce(particle p, source s){
	ofVec3f m = ofVec3f(0,0,0);
	ofVec3f dirToPull = ofVec3f(p.loc.x, p.loc.y, 0);

	dirToPull = dirToPull - s.loc;
	float distToPull = dirToPull.length();
	if(distToPull > p.deathThresh){
		float theta, F;
		F = s.mass * s.mass;
		m.x = (p.mass*p.loc.x + s.mass*s.loc.x)/(p.mass+s.mass);
		m.y = (p.mass*p.loc.y + s.mass*s.loc.y)/(p.mass+s.mass);

		//attract
		if(s.type == source::SINK)
			theta = p.findAngle( m.x - p.loc.x, m.y - p.loc.y );
		else
			theta = p.findAngle( m.x - loc.x, m.y - s.loc.y );

		m.x = (F*cos(theta)) / distToPull;
		m.y = (F*sin(theta)) / distToPull;
      
		p.angle = p.findAngle(m.x, m.y);
		p.acc.x = p.acc.x + (m.length() * cos(p.angle));
		p.acc.y = p.acc.y + (m.length() * sin(p.angle)); 
	}
	else
		p.isDead = true;
}

/*
void physicsMode::particle::applyForce(source s, bool repel){
	ofVec3f m = ofVec3f(0,0,0);
	ofVec3f dirToPull = ofVec3f(loc.x, loc.y, 0);

	dirToPull = dirToPull - s.loc;
	float distToPull = dirToPull.length();
	if(distToPull > deathThresh){
		float theta, F;
		F = mass * s.mass;
		m.x = (mass*loc.x + s.mass*s.loc.x)/(mass+s.mass);
		m.y = (mass*loc.y + s.mass*s.loc.y)/(mass+s.mass);

		if(repel)
			theta = findAngle( loc.x - m.x, loc.y -m.y);
		else
			theta = findAngle( m.x - loc.x, m.y - loc.y );
    
		m.x = (F*cos(theta)) / distToPull;
		m.y = (F*sin(theta)) / distToPull;
      
		angle = findAngle(m.x, m.y);
		acc.x = acc.x + (m.length() * cos(angle));
		acc.y = acc.y + (m.length() * sin(angle)); 
	}
	else
		isDead = true;
}
*/

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
	ofEllipse(loc.x, loc.y, 3,3);
}