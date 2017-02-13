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
		return processor->detectFace(location);
	}

	bool canSeeCircle(yarp::sig::Vector* location) {
		return processor->detectCircle(location);
	}

	bool canSeeSpecial() {
		return false;
	}

	State transitionFace(yarp::sig::Vector* location) {
		printf("State is FACE\n");
		connector->lookAt(location);
		return FACE;
	}

	State transitionCircle(yarp::sig::Vector* location) {
		printf("State is CIRCLE\n");
		connector->lookAt(location);
		return CIRCLE;
	}

	State transitionSpecial() {
		//TOOD
		return SPECIAL;
	}

	State transitionIdle() {
		printf("State is IDLE\n");
		return IDLE;
	}

	void doSpecial() {

	}

	/* StateMachine
	 * recursively fetch new image, seek items of note and switch state
	 * written by Helen Rankin
	*/
	State stateMachine(State state) {
		yarp::sig::Vector faceFocus;
		yarp::sig::Vector circleFocus;

		if (processor == NULL || connector == NULL) {
			printf("One of the important things is null!\n");
		}

		// Get and process the next frame
		processor->applyFilters(connector->getImage());

		State nextState = state;

		// Check for faces, circles, special
		bool hasFace = canSeeFace(&faceFocus);
		bool hasCircle = canSeeCircle(&circleFocus);
		bool hasSpecial = canSeeSpecial();
		switch (state) {
		case IDLE: {
			if (hasSpecial) {
				nextState = transitionSpecial();
			}
			else if (hasFace) {
				nextState = transitionFace(&faceFocus);
			}
			else if (hasCircle) {
				nextState = transitionCircle(&circleFocus);
			}
			else {
				yarp::sig::Vector neutral;
				neutral.resize(3);
				neutral[0] = 0;
				neutral[1] = 0;
				neutral[2] = 0;
				//connector->lookAt(&neutral);
			}
			break;
		}
		case CIRCLE: {
			if (hasSpecial) {
				nextState = transitionSpecial();
			}
			else if (hasFace) {
				nextState = transitionFace(&faceFocus);
			}
			else if (hasCircle) {
				connector->lookAt(&circleFocus);
			}
			else {
				nextState = transitionIdle();
			}
			break;
		}
		case FACE: {
			if (hasSpecial) {
				transitionSpecial();
			}
			else if (hasFace) {
				connector->lookAt(&faceFocus);
			}
			else {
				nextState = transitionIdle();
			}
			break;
		}
		case SPECIAL: {
			if (!hasSpecial) {
				if (hasFace) {
					nextState = transitionFace(&faceFocus);
				}
				else if (hasCircle) {
					nextState = transitionCircle(&circleFocus);
				}
				else {
					nextState = transitionIdle();
				}
			}
			break;
		}
		default: {
			break;
		}
		}
		return nextState;
	}

	bool isConnected() {
		return connector->isConnected();
	}

	Controller() {
		connector = new Connector("/test/in", "/icubSim/cam/left");
		processor = new ImageProcessor();
	}

};

	int main(char ** args) {
		Controller* controller = new Controller();
		Controller::State state = Controller::State::IDLE;

		while (controller->isConnected()) {
			state = controller->stateMachine(state);
		}

		printf("Stopping program\n");
		char* input;
		std::cin >> input;
		std::exit(0);
	}
