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

		//shadeeers
		int imgWidth, imgHeight;
		int textureRes;

		ofShader    updatePos;
		ofShader    updateVel;
		ofShader    updateRender;
		
		pingPongBuffer posPingPong;
		pingPongBuffer velPingPong;
    
		ofFbo   renderFBO;
		ofImage particleImg;


	};
struct pingPongBuffer {
	public:
	  void allocate( int _width, int _height, int _internalformat = GL_RGBA, float _dissipation = 1.0f){
        // Allocate
        for(int i = 0; i < 2; i++){
            FBOs[i].allocate(_width,_height, _internalformat );
        }
        // Clean
        clear();
        // Set everything to 0
        flag = 0;
        swap();
        flag = 0;
	  }
    void swap(){
        src = &(FBOs[(flag)%2]);
        dst = &(FBOs[++(flag)%2]);
    }  
    void clear(){
        for(int i = 0; i < 2; i++){
            FBOs[i].begin();
            ofClear(0,255);
            FBOs[i].end();
        }
    }    
    ofFbo& operator[]( int n ){ return FBOs[n];} 
    ofFbo   *src;       // Source       ->  Ping
    ofFbo   *dst;       // Destination  ->  Pong
private:
    ofFbo   FBOs[2];    // Real addresses of ping/pong FBO«s  
    int     flag;       // Integer for making a quick swap
};
