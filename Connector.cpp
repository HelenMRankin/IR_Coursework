#include "Connector.h";
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

const char* robotHeadPort = "/icubSim/head";
BufferedPort < ImageOf<PixelRgb> > readPort;
IVelocityControl *vel;
int numJoints;
Vector joints;


	Connector::Connector(char* readPortString, char* iCubInputPortString) {
		Network yarp; // set up yarp
		initializePorts(readPortString, iCubInputPortString);
		if (!initializeRobotHead()) {
			printf("Failed to initialise Robot Head\n");
			throw "Failed to initialize iCubHead";
		}
			

		printf("Connector initialized");
	}

	ImageOf<PixelRgb>* Connector::getImage() {
		return readPort.read();
	}

	void Connector::lookAt(Vector* target) {
		double x = (*target)[0];
		double y = (*target)[1];
		double conf = (*target)[2];
		x -= 320 / 2;
		y -= 240 / 2;
		double vx = x*0.1;
		double vy = -y*0.1;
		/* prepare command */
		for (int i = 0; i<numJoints; i++) {
			joints[i] = 0;
		}
		if (conf>0.5) {
			joints[3] = vy;
			joints[4] = vx;
		}
		else {
			joints[3] = 0;
			joints[4] = 0;
		}
		vel->velocityMove(joints.data());
	}

	// TODO
	void Connector::performGesture(enum gesture) {

	}

	void Connector::initializePorts(char* readPortString, char* iCubInputPortString) {
		readPort.open(readPortString);
		Network::connect(iCubInputPortString, readPortString);
	}

	bool Connector::initializeRobotHead() {
		Property options;
		options.put("device", "remote_controlboard");
		options.put("local", "/tutorial/motor/client");
		options.put("remote", "/icubSim/head");

		PolyDriver robotHead(options);
		if (!robotHead.isValid()) {
			printf("Cannot connect to robot head\n");
			return false;
		}

		robotHead.view(vel);

		if (vel == NULL) {
			printf("Cannot get interface to robot head\n");
			robotHead.close();
			return false;
		}
		
		vel->getAxes(&numJoints);
		printf("Reached getAxes\n");
		joints.resize(numJoints);
		printf("Reached resize\n");
		vel->setVelocityMode();
		printf("Reached setMode\n");
		return true;
	}

