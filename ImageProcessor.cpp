#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

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
		cv::Mat cvImage = convertYarpToCvImage(yarpImage);
	}
private:
	cv::Mat convertYarpToCvImage(ImageOf<PixelRgb> * yarpImage) {
		IplImage* tmp_ipl = (IplImage*) yarpImage->getIplImage();
		IplImage* ipl = cvCreateImage(cvSize(tmp_ipl->width,
			tmp_ipl->height), tmp_ipl->depth, 3);

		IplImage *cvImage = cvCreateImage(cvSize(yarpImage->width(),
			yarpImage->height()),
			IPL_DEPTH_8U, 3);
		return cv::cvarrToMat(cvImage);
	}

	cv::Mat applySobelDerivative(cv::Mat sourceImg) {
		// Declare & initialise variables
		cv::Mat sourceImg_grey;
		cv::Mat gradient;
		cv::Mat gradientX, gradientY;
		cv::Mat absGradientX, absGradientY;

		int ddepth = -1; //CV_16S
		double scale = 1;
		double delta = 0;
		char* test_window = "sobel filter";

		if (!sourceImg.data)
		{
			throw "No source data!\n";
		};

		cv::GaussianBlur(sourceImg, sourceImg_grey, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
		GaussianBlur(sourceImg, sourceImg_grey, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

		//Convert to grey
		cvtColor(sourceImg, sourceImg_grey, CV_BGR2GRAY);

		//test window
		cv::namedWindow(test_window);

		//calc derivatives using sobel func
		//gradient X
		//sobel parameters: void Sobel(InputArray src, OutputArray dst, int ddepth, int dx, int dy, int ksize=3, double scale=1, double delta=0, int borderType=BORDER_DEFAULT )
		Sobel(sourceImg_grey, gradientX, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
		convertScaleAbs(gradientX, absGradientX);
		//gradient Y
		Sobel(sourceImg_grey, gradientY, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
		convertScaleAbs(gradientY, absGradientY);

		//calc approx total gradient
		addWeighted(absGradientX, 0.5, absGradientY, 0.5, 0, gradient);
		imshow(test_window, gradient);
		cv::waitKey(0);
		return gradient;
	}
};