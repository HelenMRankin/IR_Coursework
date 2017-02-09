#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include "ImageProcessor.h";

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace cv;

char* rawWindow = "raw image";
char* sobelWindow = "sobel filter";

ImageProcessor::ImageProcessor() {
	//test window
	cv::namedWindow(rawWindow);
	cv::namedWindow(sobelWindow);
	printf("Processor initialized");
}

Mat ImageProcessor::convertYarpToCvImage(ImageOf<PixelRgb> * yarpImage) {
	printf("Converting image");
	IplImage* tmp_ipl = (IplImage*)yarpImage->getIplImage();


	IplImage *cvImage = cvCreateImage(cvSize(yarpImage->width(),
		yarpImage->height()),
		IPL_DEPTH_8U, 3);
	cvCvtColor((IplImage*)yarpImage->getIplImage(), cvImage, CV_RGB2BGR);
	cvShowImage("test", cvImage);
	
	return cv::cvarrToMat(cvImage, true);
}

Mat ImageProcessor::applyFilters(ImageOf<PixelRgb> * yarpImage) {
	Mat cvImage = ImageProcessor::convertYarpToCvImage(yarpImage);
	printf("ConversionDone image");
	return applySobelDerivative(cvImage);
}

Mat ImageProcessor::applySobelDerivative(Mat sourceImg) {

	
	// Declare & initialise variables
	Mat sourceImg_grey;
	Mat gradient;
	Mat gradientX, gradientY;
	Mat absGradientX, absGradientY;
	imshow(rawWindow, sourceImg);
	int ddepth = -1; //CV_16S
	double scale = 1;
	double delta = 0;

	if (!sourceImg.data)
	{
		throw "No source data!\n";
	};

	cv::GaussianBlur(sourceImg, sourceImg_grey, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
	GaussianBlur(sourceImg, sourceImg_grey, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

	//Convert to grey
	cvtColor(sourceImg, sourceImg_grey, CV_BGR2GRAY);

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
	imshow(sobelWindow, gradient);
	cv::waitKey(0);
	return gradient;
}