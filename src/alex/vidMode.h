/*-----------------------------------------------------------*
 Video Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"
#include "..\beatDetect.h"

class vidMode{
	public:
	int numParticles;

	class ParticleController{
	public:
		class Particle{
		public:
			Particle();
	//		~Particle();

			ofVec3f pos, vel;

			void update(int x, int y);
			void render(int x, int y);
		};
		ParticleController();
		ParticleController(int particles);
	//	~ParticleController();

		int numParticles;
		vector<Particle> particles;
		void update(int x, int y);
		void render(int x, int y);

		
	};

	vidMode();
	~vidMode();
	
	ParticleController p;
	ofVideoPlayer curVid;
	float vidWidth, vidHeight;
	float bpm;
	bool drawParticles;

	void setup();
	void update(int x, int y, float bpm, beatDetect bd);
	void draw(int x, int y);
	void circles();
	void updateSpeed();
	void keyPressed(int key);

	ofShader maskShader;
	ofShader shader;

	ofImage mask, topLayer, bottomLayer;
	ofTexture particleTexture, maskTexture;

	ofFbo       maskFbo;
    ofFbo       fbo;

	float seedX, seedY;
	float time;
};