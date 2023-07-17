#include "ofApp.h"

void Player::draw() {
	if (!displaySprite) {
		ofSetColor(ofColor::red);
		ofFill();
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		ofDrawTriangle(verts[0], verts[1], verts[2]);
		ofPopMatrix();
	}
	if (displaySprite) {
		ofSetColor(ofColor::white);
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		playerImg.resize(100, 100);
		playerImg.setAnchorPoint(pos.x, pos.y);
		playerImg.draw(pos.x - playerImg.getWidth() / 2, pos.y - playerImg.getHeight() / 2);
		ofPopMatrix();
	}
	if (displayHeading) {
		ofSetColor(ofColor::black);
		ofDrawLine(pos, pos + (heading() * 75));
	}
}

// Integrator for simple trajectory physics
//
void Player::integrate() {

	// init current framerate (or you can use ofGetLastFrameTime())
	//
	float framerate = ofGetFrameRate();
	float dt = 1.0 / framerate;

	// linear motion
	//
	pos += (velocity * dt);
	glm::vec3 accel = acceleration;
	accel += (force * 1.0 / mass);
	velocity += accel * dt;
	velocity *= damping;

	// angular motion
	//
	rotation += (angularVelocity * dt);
	float a = angularAcceleration;;
	a += (angularForce * 1.0 / mass);
	angularVelocity += a * dt;
	angularVelocity *= damping;

}

// Integrator for simple trajectory physics
//
void Sprite::integrate() {

	// init current framerate (or you can use ofGetLastFrameTime())
	//
	float framerate = ofGetFrameRate();
	float dt = 1.0 / framerate;

	// linear motion
	//
	pos += (velocity * dt);
	glm::vec3 accel = acceleration;
	accel += (force * 1.0 / mass);
	velocity += accel * dt;
	velocity *= damping;

	// angular motion
	//
	rotation += (angularVelocity * dt);
	float a = angularAcceleration;;
	a += (angularForce * 1.0 / mass);
	angularVelocity += a * dt;
	angularVelocity *= damping;
}

//--------------------------------------------------------------
void ofApp::setup() {
	// Explosion effect
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetBackgroundColor(ofColor::red);

	radialForce = new ImpulseRadialForce(1000);
	radialForce->setMagnitude(2500);
	radialForce->setHeight(1);
	explosionEmitter.sys->addForce(radialForce);

	explosionEmitter.setOneShot(true);
	explosionEmitter.setEmitterType(RadialEmitter);
	explosionEmitter.setGroupSize(90);

	explosionEmitter.setLifespan(1.5);
	explosionEmitter.setParticleRadius(1);
	explosionEmitter.setRate(100);
	explosionEmitter.setPosition(glm::vec3(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0));
	
	thrustEmitter.setParticleRadius(4);
	thrustEmitter.setEmitterType(DirectionalEmitter);
	thrustEmitter.setLifespan(0.3);
	thrustEmitter.setVelocity(ofVec3f(1000, 0, 0));
	thrustEmitter.setPosition(glm::vec3(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0));
	thrustEmitter.setOneShot(true);

	snipSound.load("sounds/snipsound.mp3");
	bgMusic.load("sounds/backgroundmusic.mp3");
	wallThumpSound.load("sounds/wallthumpsound.mp3");
	thrustSound.load("sounds/thrustsound.wav");
	bgMusic.setVolume(0.05);
	bgMusic.setLoop(true);
	bgMusic.play();
	isStarted = false;
	isGameOver = false;
	p = Player(glm::vec3(-50, 50, 0), glm::vec3(50, 50, 0), glm::vec3(0,
		-50, 0));
	p.pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0,
		0);
	p.rotation = 0;
	p.nEnergy = 0;

	gui.setup();
	gui.add(thrust.setup("Player Thrust", 2500, 100, 5000));
	gui.add(scaleSlider.setup("Player Scale", 0.9, 0.1, 5.0));
	gui.add(showSprite.setup("Show Player Sprite", true));
	gui.add(showAgentSprite.setup("Show Agent Sprite", true));
	gui.add(showHeading.setup("Show Player Heading Vector", false));
	gui.add(gameDifficultySlider.setup("Game Difficulty Level", 1, 1, 4));
	gui.add(nAgentsSlider.setup("Agent Spawn Count", 4, 1, 5));
	gui.add(rateSlider.setup("Agent Spawn Rate", 2, 0.5, 10));
	gui.add(lifeSlider.setup("Agent Lifespan", 10, 0, 10));
	gui.add(agentScaleSlider.setup("Agent Scale", 0.5, 0.1, 5.0));
	gui.add(agentThrust.setup("Agent Thrust", 500, 100, 1000));
	gui.add(restitution.setup("Restitution", .85, 0.0, 1.0));


	p.playerImg.load("images/pizza.png");

	foodbg.loadImage("images/foodbackground2.png");


	ofSetVerticalSync(true);

	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/razorblade.png")) {
		imageLoaded = true;
	}
	else {
		cout << "Can't open image file" << endl;
		ofExit();
	}



	// create an array of emitters and set their position;
	//

//	turret = new Emitter();

	turret = new AgentEmitter();
	turret->pos = glm::vec3(ofGetWindowWidth() - 100, 100, 0);
	turret->drawable = false;
	turret->setChildImage(defaultImage);

	bHide = false;

}

//--------------------------------------------------------------
void ofApp::update() {
	ofSeedRandom();
	thrustEmitter.update();
	explosionEmitter.update();
	
	if (!showAgentSprite) {
		for (int i = 0; i < turret->sys->sprites.size(); i++) {
			turret->sys->sprites[i].bShowImage = false;
		}
	}
	if (showAgentSprite) {
		for (int i = 0; i < turret->sys->sprites.size(); i++) {
			turret->sys->sprites[i].bShowImage = true;
		}
	}

	turret->numAgents = nAgentsSlider;
	s = triScale;  // get from slider; cast float
	p.scale = glm::vec3(s, s, s);
	p.thrust = thrust;

	// integration done here
	//
	p.isSimStarted = false;
	detectCollision();
	detectAgentCollision();
	if (isStarted)
	{
		p.isSimStarted = true;

		if (p.isForward) {
			if (!thrustSound.isPlaying()) {
				thrustSound.play();
			}
			thrustEmitter.stop();
			thrustEmitter.sys->reset();
			thrustEmitter.setPosition(p.pos);
			thrustEmitter.setVelocity(p.heading());
			thrustEmitter.start();
			p.force = p.force + p.heading() * p.thrust;
		}
		if (p.isBackward) {
			if (!thrustSound.isPlaying()) {
				thrustSound.play();
			}
			thrustEmitter.stop();
			thrustEmitter.sys->reset();
			thrustEmitter.setPosition(p.pos);
			thrustEmitter.setVelocity(p.heading());
			thrustEmitter.start();
			p.force = p.force - p.heading() * p.thrust;
		}
		if (p.isRotatingR) {
			p.angularForce = (p.thrust) * p.mass;
		}
		if (p.isRotatingL) {
			p.angularForce = -((p.thrust) * p.mass);
		}
		p.integrate();

		for (int i = 0; i < turret->sys->sprites.size(); i++) {
			turret->sys->sprites[i].isSimStarted = true;
			turret->sys->sprites[i].thrust = agentThrust;
			turret->sys->sprites[i].scaleFactorX = (float)agentScaleSlider;
			turret->sys->sprites[i].scaleFactorY = (float)agentScaleSlider;
		}
	}

	// zero out forces
	//
	p.force = glm::vec3(0, 0, 0);

	if (timeInSec == 25) {
		isGameOver = true;
		isStarted = false;
		thrustSound.stop();
		turret->stop();
		timeInSec = 0;
	}

	if (gameDifficultySlider == 2) {
		thrust = 2500;
		scaleSlider = 0.8;
		nAgentsSlider = 5;
		rateSlider = 1.7;
		lifeSlider = 9;
		agentScaleSlider = 0.5;
		agentThrust = 500;
		restitution = 0.85;
	}
	else if (gameDifficultySlider == 3) {
		thrust = 2500;
		scaleSlider = 0.7;
		nAgentsSlider = 3;
		rateSlider = 1.5;
		lifeSlider = 7;
		agentScaleSlider = 0.5;
		agentThrust = 700;
		restitution = 0.85;
	}
	else if (gameDifficultySlider == 4) {
		thrust = 2500;
		scaleSlider = 0.6;
		nAgentsSlider = 2;
		rateSlider = 1.5;
		lifeSlider = 7;
		agentScaleSlider = 0.5;
		agentThrust = 900;
		restitution = 0.85;
	}

	if (isStarted) {
		isGameOver = false;
	}
	p.scaleFactorX = (float)scaleSlider;
	p.scaleFactorY = (float)scaleSlider;
	if (isDragging) {
		p.pos = glm::vec3(mouseX, mouseY, 0);
		p.pos += posDifference;
	}

	std::stringstream ss;
	ss << "Score: " << p.nEnergy;
	energyText = ss.str();

	checkCollisions();
	velocity = 1;
	life = lifeSlider;
	rate = rateSlider;
	turret->setRate(rate);
	turret->setLifespan(life * 1000);    // convert to milliseconds 
	turret->setVelocity(glm::vec3(velocity, velocity, velocity));
	turret->update();

	elapsedTime = elapsedTime - startTime;
	cout << elapsedTime;
}


//--------------------------------------------------------------
void ofApp::draw() {

	foodbg.resize(ofGetWidth(), ofGetHeight());
	foodbg.draw(0, 0);

	explosionEmitter.draw();
	thrustEmitter.draw();

	// draw GUI if not hidden
	//
	if (!bHide) gui.draw();

	if (!showSprite) {
		p.displaySprite = false;
		p.draw();
	}
	if (showSprite) {
		p.displaySprite = true;
		p.draw();
	}
	if (!showHeading) {
		p.displayHeading = false;
		p.draw();
	}
	if (showHeading) {
		p.displayHeading = true;
		p.draw();
	}
	ofSetColor(ofColor::white);
	string startText = "Press SPACE BAR to start or reset the game";

	if (isStarted) {
		std::stringstream timess;
		elapsedTime = (ofGetElapsedTimeMillis() / 1000.0);
		timeInSec = elapsedTime * 100 / 100;
		int timeInTwoDec = (elapsedTime * 100) - (timeInSec * 100);
		timess << "Elapsed Time: " << timeInSec << "." << timeInTwoDec;
		elapsedTimeText = timess.str();
	}



	std::stringstream frameratess;
	int frameRate = (int)ofGetFrameRate();
	frameratess << "Frame Rate: " << frameRate;
	std::string frameRateText = frameratess.str();

	ofSetColor(ofColor::black);
	ofDrawBitmapString(startText + "\n", ofGetWindowWidth() - startText.length() * 9.5, 25);
	ofDrawBitmapString(energyText + "\n", ofGetWindowWidth() - startText.length() * 9.5, 50);
	ofDrawBitmapString(elapsedTimeText + "\n", ofGetWindowWidth() - startText.length() * 9.5, 75);
	ofDrawBitmapString(frameRateText + "\n", ofGetWindowWidth() - startText.length() * 9.5, 100);
	if (!isStarted && !isGameOver) {
		ofDrawBitmapString("Get cut by as many razor blades as you can in 25 seconds!", ofGetWindowWidth() / 2 - startText.length() * 5, (ofGetWindowHeight() / 2) - 60);
	}

	if (isGameOver) {
		ofDrawBitmapString("GAME OVER", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		for (int i = 0; i < turret->sys->sprites.size(); i++) {
			turret->sys->remove(i);
		}
		elapsedTime = elapsedTime - startTime;
		isStarted = false;
	}
	ofSetColor(ofColor::white);

	turret->draw();

	if (!bHide) {
		gui.draw();
	}
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (p.inside(glm::vec3(x, y, 0)) && button == 0) {
		isDragging = true;
		posDifference.x = p.pos.x - x;
		posDifference.y = p.pos.y - y;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (button == 0) {
		isDragging = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

void ofApp::keyPressed(int key) {

	switch (key) {
	case 'h':   // show/hide GUI panel
		bHide = !bHide;
		break;
	case 'f':
		bFullscreen = !bFullscreen;
		ofSetFullscreen(bFullscreen);
		break;
	case OF_KEY_LEFT:   // turn left
		p.isRotatingL = true;
		break;
	case OF_KEY_RIGHT:  // turn right
		p.isRotatingR = true;
		break;
	case OF_KEY_UP:     // go forward
		p.isForward = true;
		break;
	case OF_KEY_DOWN:   // go backward
		p.isBackward = true;
		break;
	case '1':
		thrustEmitter.sys->reset();
		thrustEmitter.start();
		break;
	case '2':
		explosionEmitter.sys->reset();
		explosionEmitter.stop();
		break;
	case ' ':
		p.nEnergy = 0;
		p.pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0,
			0);
		ofResetElapsedTimeCounter();
		if (isStarted) {
			for (int i = 0; i < turret->sys->sprites.size(); i++) {
				turret->sys->remove(i);
			}
		}
		isStarted = true;
		turret->start();
		isGameOver = false;
		startTime = ofGetElapsedTimeMillis() / 1000;
		break;
	default:
		break;
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_LEFT:   // turn left
		p.isRotatingL = false;
		p.angularForce = 0;
		break;
	case OF_KEY_RIGHT:  // turn right
		p.isRotatingR = false;
		p.angularForce = 0;
		break;
	case OF_KEY_UP:     // go forward
		thrustSound.stop();
		p.isForward = false;
		break;
	case OF_KEY_DOWN:   // go backward
		thrustSound.stop();
		p.isBackward = false;
		break;
	case ' ':
		isStarted = true;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::detectCollision() {
	glm::vec3 p0 = p.getMatrix() * glm::vec4(p.verts[0], 1);
	glm::vec3 p1 = p.getMatrix() * glm::vec4(p.verts[1], 1);
	glm::vec3 p2 = p.getMatrix() * glm::vec4(p.verts[2], 1);
	p.isCollided = false;
	if (p0.x < 0 || p0.x > ofGetWidth() || p0.y < 0 || p0.y > ofGetHeight() || // bottom right of triangle [-10, -10, 0]
		p1.x < 0 || p1.x > ofGetWidth() || p1.y < 0 || p1.y > ofGetHeight() || // heading top of triangle [0, 20, 0]
		p2.x < 0 || p2.x > ofGetWidth() || p2.y < 0 || p2.y > ofGetHeight()) { // bottom left of triangle [10, -10, 0]
		p.isCollided = true;
		if (!wallThumpSound.isPlaying()) {
			wallThumpSound.setSpeed(1);
			wallThumpSound.play();
		}
		if (p0.x < 0 || p1.x < 0 || p2.x < 0) {
			p.pos.x += 10;
			p.force += ((restitution + 1) * (glm::dot(-p.velocity, glm::vec3(-1, 0, 0)) * glm::vec3(-1, 0, 0)) * ofGetFrameRate());
		}
		if (p0.x > ofGetWidth() || p1.x > ofGetWidth() || p2.x > ofGetWidth()) {
			p.pos.x -= 10;
			p.force += ((restitution + 1) * (glm::dot(-p.velocity, glm::vec3(1, 0, 0)) * glm::vec3(1, 0, 0)) * ofGetFrameRate());
		}
		if (p0.y < 0 || p1.y < 0 || p2.y < 0) {
			p.pos.y += 10;
			p.force += ((restitution + 1) * (glm::dot(-p.velocity, glm::vec3(0, 1, 0)) * glm::vec3(0, 1, 0)) * ofGetFrameRate());
		}
		if (p0.y > ofGetHeight() || p1.y > ofGetHeight() || p2.y > ofGetHeight()) {
			p.pos.y -= 10;
			p.force += ((restitution + 1) * (glm::dot(-p.velocity, glm::vec3(0, -1, 0)) * glm::vec3(0, -1, 0)) * ofGetFrameRate());
		}
	}
}

void ofApp::detectAgentCollision() {
	for (int i = 0; i < turret->sys->sprites.size(); i++) {
		turret->sys->sprites[i].verts.push_back(glm::vec3(-50, 50, 0));
		turret->sys->sprites[i].verts.push_back(glm::vec3(50, 50, 0));
		turret->sys->sprites[i].verts.push_back(glm::vec3(0, -50, 0));

		glm::vec3 p0 = turret->sys->sprites[i].getMatrix() * glm::vec4(turret->sys->sprites[i].verts[0], 1);
		glm::vec3 p1 = turret->sys->sprites[i].getMatrix() * glm::vec4(turret->sys->sprites[i].verts[1], 1);
		glm::vec3 p2 = turret->sys->sprites[i].getMatrix() * glm::vec4(turret->sys->sprites[i].verts[2], 1);
		turret->sys->sprites[i].isCollided = false;
		if (p0.x < 0 || p0.x > ofGetWidth() || p0.y < 0 || p0.y > ofGetHeight() || // bottom right of triangle [-10, -10, 0]
			p1.x < 0 || p1.x > ofGetWidth() || p1.y < 0 || p1.y > ofGetHeight() || // heading top of triangle [0, 20, 0]
			p2.x < 0 || p2.x > ofGetWidth() || p2.y < 0 || p2.y > ofGetHeight()) { // bottom left of triangle [10, -10, 0]
			turret->sys->sprites[i].isCollided = true;
			if (p0.x < 0 || p1.x < 0 || p2.x < 0) {
				turret->sys->sprites[i].isSimStarted = true;
				turret->sys->sprites[i].pos.x += 10;
				turret->sys->sprites[i].force += ((restitution + 1) * (glm::dot(-turret->sys->sprites[i].velocity, glm::vec3(-1, 0, 0)) * glm::vec3(-1, 0, 0)) * ofGetFrameRate());
				turret->sys->sprites[i].angularForce = (turret->sys->sprites[i].thrust) * turret->sys->sprites[i].mass;
				turret->sys->sprites[i].integrate();
				turret->sys->sprites[i].force = glm::vec3(0, 0, 0);
			}
			if (p0.x > ofGetWidth() || p1.x > ofGetWidth() || p2.x > ofGetWidth()) {
				turret->sys->sprites[i].isSimStarted = true;
				turret->sys->sprites[i].pos.x -= 10;
				turret->sys->sprites[i].force += ((restitution + 1) * (glm::dot(-turret->sys->sprites[i].velocity, glm::vec3(1, 0, 0)) * glm::vec3(1, 0, 0)) * ofGetFrameRate());
				turret->sys->sprites[i].integrate();
				turret->sys->sprites[i].force = glm::vec3(0, 0, 0);
				turret->sys->sprites[i].force = turret->sys->sprites[i].force - turret->sys->sprites[i].heading() * turret->sys->sprites[i].thrust;
				turret->sys->sprites[i].integrate();
				turret->sys->sprites[i].force = glm::vec3(0, 0, 0);
			}
			if (p0.y < 0 || p1.y < 0 || p2.y < 0) {
				turret->sys->sprites[i].isSimStarted = true;
				turret->sys->sprites[i].pos.y += 10;
				turret->sys->sprites[i].force += ((restitution + 1) * (glm::dot(-turret->sys->sprites[i].velocity, glm::vec3(0, 1, 0)) * glm::vec3(0, 1, 0)) * ofGetFrameRate());
				turret->sys->sprites[i].angularForce = (turret->sys->sprites[i].thrust) * turret->sys->sprites[i].mass;
				turret->sys->sprites[i].integrate();
				turret->sys->sprites[i].force = glm::vec3(0, 0, 0);
			}
			if (p0.y > ofGetHeight() || p1.y > ofGetHeight() || p2.y > ofGetHeight()) {
				turret->sys->sprites[i].isSimStarted = true;
				turret->sys->sprites[i].pos.y -= 10;
				turret->sys->sprites[i].force += ((restitution + 1) * (glm::dot(-turret->sys->sprites[i].velocity, glm::vec3(0, -1, 0)) * glm::vec3(0, -1, 0)) * ofGetFrameRate());
				turret->sys->sprites[i].angularForce = (turret->sys->sprites[i].thrust) * turret->sys->sprites[i].mass;
				turret->sys->sprites[i].integrate();
				turret->sys->sprites[i].force = glm::vec3(0, 0, 0);
			}
		}
	
	}
}

//--------------------------------------------------------------

void ofApp::checkCollisions() {
	for (int i = 0; i < turret->sys->sprites.size(); i++) {
		glm::vec3 a0 = turret->sys->sprites[i].getMatrix() * glm::vec4(turret->sys->sprites[i].verts[0], 1);
		glm::vec3 a1 = turret->sys->sprites[i].getMatrix() * glm::vec4(turret->sys->sprites[i].verts[1], 1);
		glm::vec3 a2 = turret->sys->sprites[i].getMatrix() * glm::vec4(turret->sys->sprites[i].verts[2], 1);
		glm::vec3 p0 = p.getMatrix() * glm::vec4(p.verts[0], 1);
		glm::vec3 p1 = p.getMatrix() * glm::vec4(p.verts[1], 1);
		glm::vec3 p2 = p.getMatrix() * glm::vec4(p.verts[2], 1);
		if (p.inside(a0) || p.inside(a1) || p.inside(a2) || turret->sys->sprites[i].inside(p0) || turret->sys->sprites[i].inside(p1) || turret->sys->sprites[i].inside(p2)) {
			explosionEmitter.setPosition(turret->sys->sprites[i].pos);
			explosionEmitter.stop();
			explosionEmitter.sys->reset();
			explosionEmitter.start();
			turret->sys->remove(i);
			snipSound.setSpeed(1);
			ofSoundPlayer snippingSound = snipSound;
			snippingSound.play();
			p.nEnergy++;
		}
	}
}