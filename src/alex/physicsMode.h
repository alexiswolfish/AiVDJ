/*-----------------------------------------------------------*
 Physics Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"

class physicsMode{
	public:
		class particle{
			public:
				ofVec3f loc, vel, acc;
				float mass, magnitude, angle;
				float maxSpeed, death, deathThresh;
				int age, lifespan;
				bool isDead;

				ofColor col;

				particle(ofVec3f _loc, float m, int life);
				particle();
				~particle();

				void update();
				void render();
				float findAngle(float x, float y);
		};

		class source{
			public:
				enum Type{
					EMIT,
					SINK
				};
				ofVec3f loc;
				float radius, mass, life;
				Type type;
				ofColor col;
				std::vector<particle> particles;

				source(ofVec3f initPos, Type type);
				void render();
				void update();
				void applyForce(particle p, source s);
				void addParticles(int amt);	
		};

		physicsMode();

		void setup();
		void update();
		void render();

		vector<source> emitters;
		vector<source> sinks;

		int birthRate;
	};
