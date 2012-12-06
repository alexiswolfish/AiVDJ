/*-----------------------------------------------------------*
 Physics Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"
#include "ofxColourTheory.h"
#include "..\beatDetect.h"

class physicsMode{
	public:

		physicsMode();

		void keyPressed(int key);
		void setup(int numParticles);
		void update(beatDetect bd, float bpm);
		void render();


		void updateSources(float vol, ofColor c, bool isChanged, beatDetect bd);
		void repulseSources();

		void addParticle(float x, float y, float z);

		vector<ofVec3f> particles;
		vector<ofVec3f> sizes;

		ofVbo vbo;
		ofShader shader;
		ofEasyCam camera;
		
		float camDist;
		
		ofTexture texture;
	};
