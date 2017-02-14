#include "Connector.h";
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <thread>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace cv;

char* readPortString;
char* iCubInputPortString;
const char* robotHeadPort = "/icubSim/head";
BufferedPort < ImageOf<PixelRgb> > readPort;
IVelocityControl *vel;
IPositionControl *pos;
int numJoints;
int numRightArmJoints;
yarp::sig::Vector joints;
yarp::sig::Vector rightArmJoints;
PolyDriver* robotHead;
PolyDriver* rightArm;
Network yarpy;

	Connector::Connector(char* readPortStringIn, char* iCubInputPortStringIn) {
		readPortString = readPortStringIn;

		iCubInputPortString = iCubInputPortStringIn;
		initializePorts(readPortString, iCubInputPortString);
		if (!initializeRobotHead()) {
			printf("Failed to initialise Robot Head\n");
			throw "Failed to initialize iCubHead";
		}

		printf("Connector initialized\n");
	}

	bool Connector::isConnected() {
		bool connected = Network::isConnected(iCubInputPortString, readPortString);
		return connected;
	}

	ImageOf<PixelRgb>* Connector::getImage() {
		return readPort.read();
	}

	void Connector::lookAt(yarp::sig::Vector* target) {
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
	void Connector::performGesture() 
	{
				
	}

	void Connector::initializePorts(char* readPortString, char* iCubInputPortString) {
		readPort.open(readPortString);
		bool didConnect = Network::connect(iCubInputPortString, readPortString);
		if (didConnect) {
			printf("Connected the ports\n");
		}
		else {
			printf("Didn't connect!\n");
		}
	}

	bool Connector::initializeRobotHead() {
		Property options;
		options.put("device", "remote_controlboard");
		options.put("local", "/tutorial/motor/client");
		options.put("remote", "/icubSim/head");

		PolyDriver* potatoHead = new PolyDriver(options);
		robotHead = potatoHead;

		if (!robotHead->isValid()) {
			printf("Cannot connect to robot head\n");
			return false;
		}

		robotHead->view(vel);
		robotHead->view(pos);
		if (vel == NULL|| pos==NULL) {
			printf("Cannot get interface to robot head\n");
			robotHead->close();
			return false;
		}
		
		vel->getAxes(&numJoints);
		joints.resize(numJoints);
		pos->setPositionMode();
		for (int i = 0; i < joints.size(); i++) {
			joints[i] = 0;
		}
		pos->positionMove(joints.data());
		std::this_thread::sleep_for(std::chrono::seconds(1));
		printf("Got %i joints\n", numJoints);
		vel->setVelocityMode();
		printf("Reached setMode\n");
		return true;
	}

	bool Connector::initialiseRightArm()
	{
		Property options;
		options.put("device", "remote_controlboard");
		options.put("local", "/tutorial/motor/client");
		options.put("remote", "/icubSim/right_arm");

		PolyDriver* armJoints = new PolyDriver(options);
		rightArm = armJoints;

		if (!rightArm->isValid()) 
		{
			printf("Cannot connect to right arm\n");
			return false;
		}

		rightArm->view(pos);
		rightArm->view(vel);

		if (pos == NULL || vel == NULL) {
			printf("Cannot get interface to right arm\n");
			rightArm->close();
			return false;
		}

		vel->getAxes(&numRightArmJoints);
		joints.resize(numRightArmJoints);
		pos->setPositionMode();
		for (int i = 0; i < rightArmJoints.size(); i++) 
		{
			joints[i] = 0;
		}
		pos->positionMove(rightArmJoints.data());
		vel->setVelocityMode();
		printf("Right Arm Initialised \n");
		return true;

	}
