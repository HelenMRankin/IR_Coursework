#include "Connector.h";
#include "ImageProcessor.h";
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

enum State{IDLE, CIRCLE, FACE};
enum SeeObject{SEE_FACE, SEE_CIRCLE, SEE_NOTHING};

State currentState;

bool canSeeFace() {
	return false;
}

bool canSeeCircle() {
	return false;
}

void stateMachine(SeeObject object) {
	State nextState = IDLE;
	switch (currentState) {
	case IDLE: {
		if (canSeeFace())
			nextState = FACE;
		else if (canSeeCircle())
			nextState = CIRCLE;
	}
	case CIRCLE: {
		if (canSeeFace())
			nextState = FACE;
		else if (canSeeCircle())
			nextState = CIRCLE;
	}
	case FACE: {
		if (canSeeFace())
			nextState = FACE;
		else if (canSeeCircle())
			nextState = CIRCLE;
	}
	default: {
	}
	}
}

	int main(char ** args) {
		Connector* connector = new Connector("/test/in","/icubSim/cam/left");
		ImageProcessor* processor = new ImageProcessor();
		printf("Got processor - main\n");

		while (1) {
			printf("Getting image\n");
			ImageOf<PixelRgb> *source = connector->getImage();
			printf("Start apply filters");
			processor->applyFilters(source);
		}
			

			char* tmp;
			std::cin >> tmp;
	}
