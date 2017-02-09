#include "Connector.h";
#include "ImageProcessor.h";

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
		return 1;
	}
