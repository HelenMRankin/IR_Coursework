
#include <stdio.h>

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace cv;
class ImageProcessor {
public:
	ImageProcessor();

	Mat applyFilters(ImageOf<PixelRgb> * yarpImage);

	bool detectCircle(Mat sourceImg, Mat filteredImg, yarp::sig::Vector* location);

	void faceDetection(Mat source, Mat sobel);
private:
	Mat convertYarpToCvImage(ImageOf<PixelRgb> * yarpImage);

	Mat applySobelDerivative(Mat sourceImg);
};