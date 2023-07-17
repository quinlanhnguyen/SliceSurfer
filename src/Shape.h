#pragma once

#include "ofMain.h"

class Shape {
public:
	Shape() {}
	virtual void draw() {}
	virtual bool inside(glm::vec3 p) {
		return true;
	}
	glm::mat4 getMatrix() {
		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation),
			glm::vec3(0, 0, 1));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(scaleFactorX, scaleFactorY, 1));
		return (trans * rot * scale);
	}
	glm::vec3 pos;

	float rotation = 0.0;    // degrees 
	glm::vec3 scale = glm::vec3(1, 1, 1);
	float scaleFactorX = 1;
	float scaleFactorY = 1;
	vector<glm::vec3> verts;
};