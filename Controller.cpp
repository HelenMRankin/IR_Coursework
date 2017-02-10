#include "Connector.h";
#include "ImageProcessor.h";
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <thread>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class Controller {
public: 
	Connector* connector;
	ImageProcessor* processor;

	enum State { IDLE, CIRCLE, FACE, SPECIAL };

	bool canSeeFace(yarp::sig::Vector* location) {
		return false;
	}

	bool canSeeCircle(yarp::sig::Vector* location) {
		return false;
	}

	bool canSeeSpecial() {
		return false;
	}

	State transitionFace(yarp::sig::Vector location) {

	}

	State transitionCircle(yarp::sig::Vector location) {

	}

	State transitionSpecial() {

	}

	State transitionIdle() {

	}

	void lookAtLocation(yarp::sig::Vector location) {

	}

	void doSpecial() {

	}

	void stateMachine(State state) {
		yarp::sig::Vector faceFocus;
		yarp::sig::Vector circleFocus;

		State nextState = state;

		bool hasFace = canSeeFace(&faceFocus);
		bool hasCircle = canSeeCircle(&circleFocus);
		bool hasSpecial = canSeeSpecial();
		switch (state) {
		case IDLE: {
			if (hasSpecial) {
				nextState = transitionSpecial();
			}
			else if (hasFace) {
				nextState = transitionFace(faceFocus);
			}
			else if (hasCircle) {
				nextState = transitionCircle(circleFocus);
			}
			break;
		}
		case CIRCLE: {
			if (hasSpecial) {
				nextState = transitionSpecial();
			}
			else if (hasFace) {
				nextState = transitionFace(faceFocus);
			}
			else if (hasCircle) {
				lookAtLocation(circleFocus);
			}
			else {
				transitionIdle();
			}
			break;
		}
		case FACE: {
			if (hasSpecial) {
				transitionSpecial();
			}
			else if (hasFace) {
				lookAtLocation(faceFocus);
			}
			else {
				transitionIdle();
			}
			break;
		}
		case SPECIAL: {
			if (!hasSpecial) {
				if (hasFace) {
					nextState = transitionFace(faceFocus);
				}
				else if (hasCircle) {
					nextState = transitionCircle(circleFocus);
				}
				else {
					transitionIdle();
				}
			}
			break;
		}
		default: {
			break;
		}
		}

		stateMachine(nextState);
	}

	Controller() {
		connector = new Connector("/test/in", "/icubSim/cam/left");
		processor = new ImageProcessor();
		std::thread machine (stateMachine, IDLE);
	}

};


	int main(char ** args) {
		Controller* controller = new Controller();

	}
