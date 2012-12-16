#include "physicsMode.h"

/*------------Utils------------------*/
void physicsMode::generateColors(ColourShade cs){
	colors.clear();
	for(int i=0; i<100; i++){
		colors.push_back(colorGen.getColor(50, colorGen.getColourConstraints(cs)));
	}
}
/*-----------Emitter class-------------*/
physicsMode::Emitter::Emitter(){
}
/*-----------dParticle class-------------*/
physicsMode::dParticle::dParticle(){
	age = 0;
	lifeSpan = 100;

}
/*-----------Source class-------------*/
physicsMode::Source::Source(){
	pos = ofVec3f(0,0,0);
	vel = ofVec3f(0,0,0);
	radius = 100;
}
physicsMode::Source::Source(ofVec3f loc){
	pos = loc;
	vel = ofVec3f(0,0,0);
}

void physicsMode::Source::render(){
	ofPushMatrix();
	ofSetColor(c);
	ofPopMatrix();
}

/*-----------Particle class-------------*/
physicsMode::Particle::Particle(){
}
void physicsMode::Particle::update(beatDetect bd, float bpm){
	
	if(bd.isSnare()){
		vel += bpm/8000.0f;
	}
	vel.limit(bpm/7000.0f);
	theta += vel;
	vel*=0.96f;


	oPos.x = cos( theta.x ) * cos( theta.y );
	oPos.y = sin( theta.x );
	oPos.z = cos( theta.x ) * sin( theta.y );
	oPos *= 100;

	pos = oPos * bd.getVolume()*20;

}

/*-----------Physics Mode-------------*/
physicsMode::physicsMode(){
	sources.push_back(Source());
}

void physicsMode::setup(int numParticles){
	//ofBackgroundHex(0x000000);
	generateColors(CT_SOFT);
	setColorScheme(6, colors);
	//ofBackground(bg);
	
	
	// load the texure
	ofDisableArbTex();
	ofLoadImage(texture, "dot.png");
	ofLoadImage(srcTexture, "spark.png");

	camDist  = 1800;
	camera.setDistance(camDist);

	minRad = 100;
	maxRad = 1000;

	for(int i = 0; i<numParticles; i++ ) {
		
		Particle p;
		p.theta = ofVec3f(ofRandom(0, TWO_PI), ofRandom(0, TWO_PI), 0);
		p.c = colorGen.getRandom(colors);

		ofVec3f loc;
		loc.x = cos( p.theta.x ) * cos( p.theta.y );
		loc.y = sin( p.theta.x );
		loc.z = cos( p.theta.x ) * sin( p.theta.y );
		loc *= minRad;
		locs.push_back(loc);

		p.pos = loc;
		p.oPos = loc;
		p.radius = ofRandom(5, 50);
		sizes.push_back(ofVec3f(p.radius));

		particles.push_back(p);
	}

	int total = (int)locs.size();
	vbo.setVertexData(&locs[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
	
	if(shader.load("shader")) {
		printf("Shader is loaded\n"); 
	}

}

void physicsMode::update(beatDetect bd, float bpm){

	for (int i=0; i<particles.size(); i++) {
		particles[i].update(bd, bpm);
	}

	for (int i=0; i<sources.size(); i++){
		if(bd.isSnare())
			sources[i].c = colorGen.getRandom(colors);
		sources[i].radius = bd.magnitude_average[0]*110;
	}

	updateShaderArrays();

	for( int i =0; i<sizes.size(); i++ ){
		sizes[i] = ofVec3f( particles[i].radius -3  + bd.magnitude_average[(i%256)]*bd.magnitude_average[(i%256)]*18.0f);
	}
	vbo.setVertexData(&locs[0], (int)locs.size(), GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0],(int)locs.size(), GL_STATIC_DRAW);
}

void physicsMode::render(beatDetect bd, float bpm){
	glDepthMask(GL_FALSE);
	
	if(bd.isBeat(27))
		ofSetColor(colorGen.getColor(ofRandom(0,255),  colorGen.getColourConstraints(particleTheme)));
	else
		ofSetColor(particleMain);
	//glow
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();

	//bind shader/camera
	shader.begin();
	camera.begin();

	texture.bind();
	vbo.draw(GL_POINTS, 0, (int)locs.size());
	texture.unbind();
	
	camera.end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();

	//draw rays
	ofEnableAlphaBlending();
	camera.begin();
	for(int s=0; s<sources.size(); s++){

		Source curS = sources[s];
		//render sources
		curS.render();
		float rad =  curS.radius*1.9;
		srcTexture.draw(curS.pos - rad/2, rad, rad);

		//render rays;
		for (int i=0; i<particles.size(); i++) {
			ofVec3f mid = particles[i].oPos;
			mid.normalize();
			mid *= sources[s].radius;
			ofSetColor(255, 80);
			ofLine(mid, particles[i].pos);
		} 
	}
	camera.end();
}

void physicsMode::addParticle(float x, float y, float z) {
	ofVec3f p(x, y, z);
	locs.push_back(p);
	
	// we are passing the size in as a normal x position
	float size = ofRandom(5, 50);
	sizes.push_back(ofVec3f(size));
}

void physicsMode::updateShaderArrays(){
	for( int i =0; i<sizes.size(); i++ ){

		sizes[i] = ofVec3f( particles[i].radius);
		locs[i] = particles[i].pos;
	}
}

void physicsMode::keyPressed(int key){
	if(key == OF_KEY_UP) {
		camDist -= 10;
	}
	if(key == OF_KEY_DOWN) {
		camDist += 10;
	}
	camera.setDistance(camDist);

	ofBackground(bg);
	generateColors(centerTheme);
}

void physicsMode::setColorScheme(int s, vector<ofColor> c){
	// light,dark,bright,weak,neutral,fresh,soft,hard,warm,cool,intense
	switch(s){
	case 0: //light
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_LIGHT;
		particleTheme = CT_LIGHT;
		break;
	case 1:
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_DARK;
		particleTheme = CT_BRIGHT;
		break;
	case 2: //bright
		bg = ofColor(0,130,255);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_BRIGHT;
		particleTheme = CT_WARM;
	case 3:
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getLightest(c);
		centerTheme = CT_WEAK;
		particleTheme = CT_SOFT;
		break;
	case 4:
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_NEUTRAL;
		particleTheme = CT_WEAK;
		break;
	case 5:
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_FRESH;
		particleTheme = CT_FRESH;
		break;
	case 6: //soft mauve
		bg = ofColor(79, 55, 56);
		particleMain = ofColor(255, 100, 90);
		centerTheme = CT_SOFT;
		particleTheme = CT_LIGHT;
		break;
	case 7: //hard
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_HARD;
		particleTheme = CT_HARD;
		break;
	case 8: //warm
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_WARM;
		particleTheme = CT_WARM;
		break;
	case 9: //cool
		bg = colorGen.getDarkest(c);
		particleMain = colorGen.getRandom(c);
		centerTheme = CT_COOL;
		particleTheme = CT_COOL;
		break;
	case 10: //intense
		bg = ofColor(0,0,0);
		particleMain = ofColor(255,100,90);
		centerTheme = CT_INTENSE;
		particleTheme = CT_BRIGHT;
		break;
	default:
		bg = ofColor(79, 55, 56);
		particleMain = ofColor(255, 100, 90);
		centerTheme = CT_SOFT;
		particleTheme = CT_LIGHT;
	};
	ofBackground(bg);
	generateColors(centerTheme);
}



