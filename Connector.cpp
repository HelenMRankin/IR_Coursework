#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
class Connector {
public:
	Connector(char* readPortString, char* iCubInputPortString) {
		Network yarp; // set up yarp
		initializePorts(readPortString, iCubInputPortString);
		if (!initializeRobotHead())
			throw "Failed to initialize iCubHead";
	}

	ImageOf<PixelRgb>* getImage() {
		return readPort.read();
	}

	void lookAt(Vector* target) {
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
	void performGesture(enum gesture) {

	}
private:
	void initializePorts(char* readPortString, char* iCubInputPortString) {
		readPort.open(readPortString);
		Network::connect(iCubInputPortString, readPortString);
	}

	int initializeRobotHead() {
		Property options;
		options.put("device", "remote_controlboard");
		options.put("local", "/tutorial/motor/client");
		options.put("remote", "/icubSim/head");

		PolyDriver robotHead(options);
		if (!robotHead.isValid()) {
			printf("Cannot connect to robot head\n");
			return 1;
		}

		robotHead.view(vel);

		if (vel == NULL) {
			printf("Cannot get interface to robot head\n");
			robotHead.close();
			return 1;
		}
		
		vel->getAxes(&numJoints);
		joints.resize(numJoints);
		vel->setVelocityMode();
		return 0;
	}

	const char* robotHeadPort = "/icubSim/head";
	BufferedPort < ImageOf<PixelRgb> > readPort;
	IVelocityControl *vel;
	int numJoints;
	Vector joints;
};