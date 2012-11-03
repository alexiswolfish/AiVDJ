/*-----------------------------------------------------------*
 Physics Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"

class physicsMode{
	public:
		class source{
			public:
				enum Type{
					EMIT,
					SINK,
					ORBIT
				};
				ofVec3f loc;
				float radius, mass, life;
				Type type;
				ofColor col;

				source(ofVec3f initPos, Type type);
				void render();
				void update();
		};

		class particle{
			public:
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
				void pullToCenter(ofVec3f center);
				void applyForce(source a, float range);
				float findAngle(float x, float y);
		};

		vector<source> sources;
		vector<particle> particles;
		int birthRate, maxParticles;

		physicsMode();

		void setup();
		void update();
		void render();
		void updateSources(float vol);

		void addParticles(int amt);	

		void mousePressed(source::Type t, ofVec3f loc);
	};
