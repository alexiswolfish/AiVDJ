/*-----------------------------------------------------------*
 Physics Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"
#include "ofxColourTheory.h"

class physicsMode{
	public:
		
class source {
		public:
			class particle{
				public:
					ofxColourTheory colorGen;
					ofVec3f pLoc, loc, vel, acc;
					float mass, magnitude, angle;
					float maxSpeed, death;
					int age, lifespan;
					bool isDead;
					ofColor col;

					particle(ofVec3f _loc, float m, int life);

					particle();
					~particle();

					void update();
					void render();
					void pull(source s, float range);
					void push(source s, float range);
					void push(particle p, float range);
					void orbit(source s, float range);
					void applyForce(source a, float range);
					float findAngle(float x, float y);
				};

			enum Type{
				EMIT,
				SINK,
				ORBIT
			};
			ofVec3f loc, vel, acc;
			float radius, mass, energy, charge;
			float p,f,theta;
			Type type;
			ofColor col;

			vector<particle> mParticles;

			//functions that operate on other sources
			source(ofVec3f initPos, Type type, ofImage s);
			void render();
			void update();
			void attract(source s, float range);
			void pullToCenter(float distThresh);
			float findAngle(float x1, float y1, float x2, float y2);
			float findAngle(float x, float y);

			//particle controller functions
			void repulseParticles();
			vector<particle> addParticles(int num);
			void updateParticles();
			void renderParticles();
			ofImage spark;
	};

		vector<source> sources;
		vector<physicsMode::source::particle> particles;
		int birthRate, maxParticles;

		physicsMode();

		void setup();
		void update();
		void render();
		void updateSources(float vol, ofColor c, bool isChanged);
		void repulseSources();

		void addParticles(int amt);	

		void mousePressed(physicsMode::source::Type t, ofVec3f loc);
		
		ofImage srcImg;
	};
