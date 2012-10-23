#include "physicsMode.h"


physicsMode::physicsMode(){
}

physicsMode::~physicsMode(){
}

physicsMode::particle::particle(ofVec3f _loc, float m){
	loc = _loc;
	mass =m;
	maxSpeed = 30;
	magnitude = 0;
	angle = 0;
	death = 0.1;

	vel = ofVec3f(0,0,0);
	acc = ofVec3f(0,0,0);
}

physicsMode::particle::~particle(){
}

void physicsMode::particle::update(){
	vel = vel+acc;
	vel.limit(maxSpeed);
	loc = loc + acc;
	acc = acc * death;
}

void physicsMode::particle::repel(source s){
	ofVec3f m = ofVec3f(0,0,0);
	ofVec3f dirToPull = ofVec3f(loc.x, loc.y, 0);

	dirToPull = dirToPull - s.loc;
	float distToPull = dirToPull.length();


}
