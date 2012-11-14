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
				ofVec3f loc, vel, acc;
				float radius, mass, energy, charge;
				float p,f,theta;
				Type type;
				ofColor col;

				source(ofVec3f initPos, Type type, ofImage s);
				void render();
				void update();
				void attract(source s, float range);
				void pullToCenter(float distThresh);
				float findAngle(float x1, float y1, float x2, float y2);
				float findAngle(float x, float y);

				ofImage spark;
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
		void updateSources(float vol, ofColor c, bool isChanged);
		void repulseSources();

		void addParticles(int amt);	

		void mousePressed(source::Type t, ofVec3f loc);
		
		ofImage srcImg;
	};
