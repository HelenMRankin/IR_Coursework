#include <stdio.h>

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
class Connector {
public:
	Connector(char* readPortString, char* iCubInputPortString);

	ImageOf<PixelRgb>* getImage();

	bool isConnected();

	void lookAt(yarp::sig::Vector* target);

	// TODO
	void performGesture(enum gesture);
private: 
	void initializePorts(char* readPortString, char* iCubInputPortString);

	bool initializeRobotHead();
};