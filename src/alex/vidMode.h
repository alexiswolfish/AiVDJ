/*-----------------------------------------------------------*
 Video Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"

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

		ofVideoGrabber 		vidGrabber;
		unsigned char * 	videoInverted;
		ofTexture			videoTexture;
		int 				camWidth;
		int 				camHeight;

	void setup();
	void update(int x, int y);
	void draw(int x, int y);
};