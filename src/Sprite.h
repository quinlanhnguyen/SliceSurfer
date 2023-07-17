#pragma once

#include "Shape.h"



class TriangleShape : public Shape {
public:
	TriangleShape() {
	}
	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	virtual void draw();
	virtual bool inside(const glm::vec3 p);
	virtual bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

	void set(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
		verts.clear();
		verts.push_back(v1);
		verts.push_back(v2);
		verts.push_back(v3);
	}
};

class Player : public TriangleShape {
public:
	Player() {}
	Player(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	void draw();

	glm::vec3 Player::heading() {
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::vec3 heading = rot * glm::vec4(0, -1, 0, 1);
		return heading;
	}

	//  Integrator Function;
	//
	void integrate();

	// Physics data goes here  (for integrate() );
	//
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 force = glm::vec3(0, 0, 0);
	float mass = 1.0;
	float damping = .95;
	float angularForce = 0;
	float angularVelocity = 0.0;
	float angularAcceleration = 0.0;
	bool bThrust = false;
	float prevDist = 0;
	float thrust = 0;

	ofImage playerImg;
	bool displaySprite = false;
	bool displayHeading = false;
	int nEnergy = 0;


	bool bSelected = false;
	bool isCollided = false;
	bool isSimStarted = false;
	// Player Movement
	//
	bool isForward = false;
	bool isBackward = false;
	bool isRotatingL = false;
	bool isRotatingR = false;
};

class Sprite : public TriangleShape {
public:
	Sprite() {}
	Sprite(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	// some functions for highlighting when selected
	//
	void draw() {
		if (bShowImage) {
			ofPushMatrix();
			ofSetColor(ofColor::white);
			ofMultMatrix(getMatrix());
			spriteImage.draw(-spriteImage.getWidth() / 2, -spriteImage.getHeight() / 2.0);
			spriteImage.resize(100, 100);
			ofPopMatrix();
		}
		else
		{
			if (bHighlight) ofSetColor(ofColor::white);
			else ofSetColor(ofColor::yellow);
			TriangleShape::draw();
		}
	}

	float age() {
		return (ofGetElapsedTimeMillis() - birthtime);
	}

	void setImage(ofImage img) {
		spriteImage = img;
		bShowImage = true;
		width = img.getWidth();
		height = img.getHeight();
	}

	bool inside(const glm::vec3 p);

	void setSelected(bool state) { bSelected = state; }
	void setHighlight(bool state) { bHighlight = state; }
	bool isSelected() { return bSelected; }
	bool isHighlight() { return bHighlight; }


	glm::vec3 Sprite::heading() {
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::vec3 heading = rot * glm::vec4(0, -1, 0, 1);
		return heading;
	}

	bool bHighlight = false;
	bool bSelected = false;
	bool bShowImage = false;

	glm::vec3 velocity = glm::vec3(0, 0, 0);
	float birthtime = 0; // elapsed time in ms
	float lifespan = -1;  //  time in ms
	string name = "UnammedSprite";
	float width = 40;
	float height = 40;
	ofImage spriteImage;


	//  Integrator Function;
	//
	void integrate();

	// Physics data goes here  (for integrate() );
	//
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 force = glm::vec3(0, 0, 0);
	float mass = 1.0;
	float damping = .99;
	float angularForce = 0;
	float angularVelocity = 0.0;
	float angularAcceleration = 0.0;
	bool bThrust = false;
	float prevDist = 0;
	float thrust = 0;

	ofImage rocket;
	bool displaySprite = false;
	bool displayHeading = false;
	int nEnergy = 0;

	bool isCollided = false;
	bool isSimStarted = false;

	// Player Movement
	//
	bool isForward = false;
	bool isBackward = false;
	bool isRotatingL = false;
	bool isRotatingR = false;
};

