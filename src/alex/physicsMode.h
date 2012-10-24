/*-----------------------------------------------------------*
 Physics Mode for AiVDJ

 created by: Alex Wolfe
 *-----------------------------------------------------------*/

#include "ofMain.h"

class physicsMode{
	public:
		physicsMode();

		void update();
		void render();

		class source{
			public:
				ofVec3f loc;
				float radius, mass;
				ofColor col;

				source();
		};
		class particle{
			public:
				ofVec3f loc, vel, acc;
				float mass, magnitude, angle;
				float maxSpeed, death;
				ofColor col;

				particle(ofVec3f _loc, float m);

				void update();
				void repel(source s);
				void attract(source s);

				void render();
				void findAngle();

		};

};

