#include "Connector.h";
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace cv;

char* readPortString;
char* iCubInputPortString;
const char* robotHeadPort = "/icubSim/head";
BufferedPort < ImageOf<PixelRgb> > readPort;
BufferedPort <ImageOf<PixelRgb>> writePort;
IVelocityControl *vel;
int numJoints;
yarp::sig::Vector joints;
PolyDriver* robotHead;
Network yarpy;

	Connector::Connector(char* readPortStringIn, char* iCubInputPortStringIn) {
		readPortString = readPortStringIn;

		iCubInputPortString = iCubInputPortStringIn;
		initializePorts(readPortString, iCubInputPortString);
		if (!initializeRobotHead()) {
			printf("Failed to initialise Robot Head\n");
			throw "Failed to initialize iCubHead";
		}
		displayImage("H:\\.windows_settings\\Desktop\\iCub\\IR_Coursework\\face.jpg");

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
	void Connector::performGesture(enum gesture) {

	}

	void Connector::initializePorts(char* readPortString, char* iCubInputPortString) {
		readPort.open(readPortString);
		writePort.open("/test/out");
		bool didConnect = Network::connect(iCubInputPortString, readPortString);
		Network::connect("/test/out", "/icubSim/texture/screen");

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

		if (vel == NULL) {
			printf("Cannot get interface to robot head\n");
			robotHead->close();
			return false;
		}
		
		vel->getAxes(&numJoints);
		joints.resize(numJoints);
		printf("Got %i joints\n", numJoints);
		vel->setVelocityMode();
		printf("Reached setMode\n");
		return true;
	}

	void Connector::displayImage(char* filePath) {
		IplImage* imageBGR = cvLoadImage(filePath);
		if (!imageBGR->imageData) {
			printf("Could not load image\n");
		}
		printf("Loaded image\n");
		char* input;
		try {
			IplImage* image;
			ImageOf<PixelRgb> output;
			printf("Converting color\n");
			cvCvtColor(imageBGR, imageBGR, CV_BGR2RGB);
			printf("Wrapping image\n");
			output.wrapIplImage(imageBGR);
			
			printf("Preparing image\n");
			writePort.prepare() = output;
		}
		catch (...) {
			printf("Failed!");
			std::cin >> input;
		}
		std::cin >> input;
		
		writePort.write();
	}