#include "physicsMode.h"

physicsMode::physicsMode(){

}

void physicsMode::setup(int numParticles){
	ofBackgroundHex(0x000000);
	ofSetVerticalSync(true);

	// load the texure
	ofDisableArbTex();
	ofLoadImage(texture, "dot.png");

	camDist  = 1800;
	camera.setDistance(camDist);

	float radius = 200;

	for(int i = 0; i<numParticles; i++ ) {
		
		float theta1 = ofRandom(0, TWO_PI);
		float theta2 = ofRandom(0, TWO_PI);
		
		ofVec3f p;
		p.x = cos( theta1 ) * cos( theta2 );
		p.y = sin( theta1 );
		p.z = cos( theta1 ) * sin( theta2 );
		p *= radius;
		
		addParticle(p.x, p.y, p.z);
	}

	int total = (int)particles.size();
	vbo.setVertexData(&particles[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
	
	if(shader.load("shader")) {
		printf("Shader is loaded\n"); 
	}

}

void physicsMode::update(beatDetect bd, float bpm){

}

void physicsMode::render(){
	glDepthMask(GL_FALSE);
	ofSetColor(255, 100, 90);

	//glow
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();

	//bind shader/camera
	shader.begin();
	camera.begin();

	texture.bind();
	vbo.draw(GL_POINTS, 0, (int)particles.size());
	texture.unbind();
	
	camera.end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();

	//draw rays
	ofEnableAlphaBlending();
	camera.begin();
	for (int i=0; i<particles.size(); i++) {
		ofSetColor(255, 80);
		ofVec3f mid = particles[i];
		mid.normalize();
		mid *= 100;
		ofLine(particles[i], mid);
	} 
	camera.end();
}

void physicsMode::addParticle(float x, float y, float z) {
	ofVec3f p(x, y, z);
	particles.push_back(p);
	
	// we are passing the size in as a normal x position
	float size = ofRandom(5, 50);
	sizes.push_back(ofVec3f(size));
}

void physicsMode::keyPressed(int key){
	if(key == OF_KEY_UP) {
		camDist -= 10;
	}
	if(key == OF_KEY_DOWN) {
		camDist += 10;
	}
	camera.setDistance(camDist);
}



