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
	bool isRunning;
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
		printf("Now looking at face\n");
		connector->lookAt(location);
		return FACE;
	}

	State transitionCircle(yarp::sig::Vector* location) {
		printf("Now looking at circle\n");
		connector->lookAt(location);
		return CIRCLE;
	}

	State transitionSpecial() {
		//TOOD
		return SPECIAL;
	}

	State transitionIdle() {
		return IDLE;
	}

	void lookAtLocation(yarp::sig::Vector location) {

	}

	void doSpecial() {

	}

	/* StateMachine
	 * recursively fetch new image, seek items of note and switch state
	 * written by Helen Rankin
	*/
	void stateMachine(State state) {
		yarp::sig::Vector faceFocus;
		yarp::sig::Vector circleFocus;

		if (!isRunning) {
			return;
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
					nextState = transitionFace(&faceFocus);
				}
				else if (hasCircle) {
					nextState = transitionCircle(&circleFocus);
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
		// Recurse
		stateMachine(nextState);
	}

	void stopRunning() {
		isRunning = false;
		// TODO disconnects
	}

	Controller() {
		connector = new Connector("/test/in", "/icubSim/cam/left");
		processor = new ImageProcessor();
	}

};

void startStateMachine(Controller *controller) {
	controller->stateMachine(Controller::State::IDLE);
}

	int main(char ** args) {
		Controller* controller = new Controller();
		std::string input;
		std::cin >> input;

		std::thread myThread (startStateMachine,controller);
		while (input != "exit") {
			std::cin >> input;
		}
		controller->stopRunning();
		myThread.join();
		std::exit(0);
	}
