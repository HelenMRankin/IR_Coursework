
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
private:
	Mat convertYarpToCvImage(ImageOf<PixelRgb> * yarpImage);

	Mat applySobelDerivative(Mat sourceImg);
};