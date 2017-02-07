#include <stdio.h>

#include <cv.h>
#include <cvaux.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
class ImageProcessor {
public:
	ImageProcessor() {
	
	}

	// I'm guessing the filters take in an inputarray and return the same? If not feel free to change!

	IplImage *applyFilters(ImageOf<PixelRgb> * yarpImage) {
		IplImage * cvImage = convertYarpToCvImage(yarpImage);
	}
private:
	IplImage* convertYarpToCvImage(ImageOf<PixelRgb> * yarpImage) {
		IplImage* tmp_ipl = (IplImage*) yarpImage->getIplImage();
		IplImage* ipl = cvCreateImage(cvSize(tmp_ipl->width,
			tmp_ipl->height), tmp_ipl->depth, 3);

		IplImage *cvImage = cvCreateImage(cvSize(yarpImage->width(),
			yarpImage->height()),
			IPL_DEPTH_8U, 3);
		return cvImage;
	}
};