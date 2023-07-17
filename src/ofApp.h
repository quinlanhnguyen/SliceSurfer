#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"
#include "Particle.h"
#include "ParticleEmitter.h"

class Agent : public Sprite {
public:
	int rndRotation = ofRandom(0, 360);
	Agent() {

		// Print a debug message to show that the Agent's constructor is being 
		// called and that the emitter is, in fact, emitting Agents.
		//
		cout << "Hello, I am Agent: " << ofGetElapsedTimeMillis() << endl;
	}

};

class AgentEmitter : public Emitter {
public:
	int numAgents;
	// Note: you need to overload parent class' moveSprite if you want custom 
	// behavior. In this case, my overloaded function calls the parent class'
	// move so I still get change in position, but then I add a rotation to it.
	//
	void moveSprite(Sprite* sprite) {
		sprite->isSimStarted = true;
		sprite->force = sprite->force + sprite->heading() * sprite->thrust;
		sprite->angularForce = (sprite->thrust/10) * sprite->mass;
		sprite->integrate();
		sprite->force = glm::vec3(0, 0, 0);
	}

	// You want the AgentEmitter to emit Agents and not sprites, so we just overload
	// the spawn function to spawn Agents instead.  The Agent is a type of Sprite, so
	// the Emitter is able to support Agents now because of polymorphism.
	// 
	void spawnSprite() {
		Agent agent;
		if (haveChildImage) agent.setImage(childImage);
		agent.velocity = velocity;
		agent.lifespan = lifespan;
		agent.pos = pos;
		agent.birthtime = ofGetElapsedTimeMillis();
		for (int i = 0; i < numAgents; i++) {
			int x = ofRandom(0, ofGetWidth());
			int y = ofRandom(0, ofGetHeight());
			agent.pos.x = x;
			agent.pos.y = y;
			agent.rotation = agent.rndRotation;
			sys->add(agent);
		}
	}
};


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void checkCollisions();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void detectCollision();
	void detectAgentCollision();

	AgentEmitter* turret = NULL;

	ofImage defaultImage;
	ofVec3f mouse_last;
	bool imageLoaded;

	bool bHide = false;

	ofxFloatSlider rateSlider;
	ofxFloatSlider lifeSlider;
	ofxFloatSlider velocitySlider;
	float velocity;
	float rate;
	float life;
	ofxLabel screenSize;


	ofxPanel gui;

	bool isDragging;
	bool isRotating;
	bool isScalingX;
	bool isScalingY;
	glm::vec3 posDifference;

	Player p;
	ofxFloatSlider scaleSlider;
	ofxFloatSlider agentScaleSlider;
	ofxIntSlider nEnergySlider;
	ofxIntSlider nAgentsSlider;
	ofxIntSlider aLifeSpanSlider;
	ofxFloatSlider aSpawnRateSlider;
	ofxIntSlider gameDifficultySlider;
	ofxToggle showSprite;
	ofxToggle showAgentSprite;
	ofxToggle showHeading;
	ofxFloatSlider thrust;
	ofxFloatSlider agentThrust;
	ofxFloatSlider triScale;
	ofxFloatSlider restitution;

	ofImage foodbg;
	ofxToggle drawHeading;
	ofSoundPlayer snipSound;
	ofSoundPlayer bgMusic;
	ofSoundPlayer wallThumpSound;
	ofSoundPlayer thrustSound;

	bool isStarted;
	bool isGameOver;
	float elapsedTime;
	float startTime;
	std::string elapsedTimeText;
	std::string energyText;

	glm::vec3 mouseLast;
	bool bFullscreen = false;

	float s;

	int timeInSec;

	ParticleEmitter explosionEmitter;
	ImpulseRadialForce* radialForce;
	ImpulseRadialHeightForce* radialHeightForce;
	ParticleEmitter thrustEmitter;
};
