
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

	void applyFilters(ImageOf<PixelRgb> * yarpImage);

	bool detectCircle(yarp::sig::Vector* location);

	bool detectFace(yarp::sig::Vector* location);
private:
	Mat convertYarpToCvImage(ImageOf<PixelRgb> * yarpImage);

	Mat applyBlur(Mat sourceImg_grey);

	Mat applyCanny(Mat sourceImg_grey);

	Mat applySobelDerivative(Mat sourceImg);
};