/*-----------------------------------------------------------*
 Physics Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"
#include "ofxColourTheory.h"
#include "..\beatDetect.h"

class physicsMode {
	public:

		class Source{
		public:
			Source();
			Source(ofVec3f loc);
			ofColor c;
			ofVec3f pos, vel;
			float radius;

			void update();
			void render();
		};
		class Particle : public Source{
		public:
			Particle();
	//		~Particle();

		//	ofVec3f ppos, pvel;
		//	float pradius;
			//float theta1, theta2;
			ofVec3f theta;
			ofVec3f oPos;
		

			void update(beatDetect bd, float bpm);
			void render(int x, int y);
		};
		class Emitter : public Source{
		public:
			void render();

		};
		class dParticle : public Emitter{
		public:
		};

		physicsMode();
		
	/*------------Utils------------------*/
		void generateColors(ColourShade cs);
		void keyPressed(int key);


		void setup(int numParticles);
		void update(beatDetect bd, float bpm);
		void render(beatDetect bd, float bpm);

		void updateShaderArrays();

		void updateSources(float vol, ofColor c, bool isChanged, beatDetect bd);
		void repulseSources();

		void addParticle(float x, float y, float z);

		void setColorScheme(int s);

		vector<ofVec3f> locs;
		vector<ofVec3f> sizes;

		vector<Particle> particles;
		vector<Source> sources;
		vector<ofColor> colors;

		ofVbo vbo;
		ofShader shader;
		ofEasyCam camera;
		ofxColourTheory colorGen;

		float camDist;
		float minRad, maxRad;
		ofTexture texture;
		ofTexture srcTexture;


		ofColor bg, particleMain;
		ColourShade centerTheme, particleTheme;
	};
