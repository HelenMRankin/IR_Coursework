#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "ImageProcessor.h";

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace cv;

char* rawWindow = "raw image";
char* blurWindow = "blur ";
char* sobelWindow = "sobel filter";
char* detectFace = "Haar Cascades Face detection";
char* detectCircle = "Hough Circle Transform Demo";

ImageProcessor::ImageProcessor() {
	//test window
	cv::namedWindow(rawWindow);
	cv::namedWindow(sobelWindow);
	printf("Processor initialized\n");
}

Mat ImageProcessor::convertYarpToCvImage(ImageOf<PixelRgb> * yarpImage) {
	printf("Converting image\n");
	IplImage* tmp_ipl = (IplImage*)yarpImage->getIplImage();


	IplImage *cvImage = cvCreateImage(cvSize(yarpImage->width(),
		yarpImage->height()),
		IPL_DEPTH_8U, 3);
	cvCvtColor((IplImage*)yarpImage->getIplImage(), cvImage, CV_RGB2BGR);
	
	return cv::cvarrToMat(cvImage, true);
}

Mat ImageProcessor::applyFilters(ImageOf<PixelRgb> * yarpImage) {
	Mat cvImage = ImageProcessor::convertYarpToCvImage(yarpImage);
	printf("Conversion done\n");
	yarp::sig::Vector location;
	
	Mat saliency= applySobelDerivative(cvImage);
	detectCircle(cvImage, saliency, &location);
	return saliency;
}

bool ImageProcessor::detectCircle(Mat sourceImage, Mat filteredImage, yarp::sig::Vector* location) {
	vector<Vec3f> circles;
	cv::HoughCircles(filteredImage, circles, CV_HOUGH_GRADIENT, 2, sourceImage.rows / 8, 100, 50);
	printf("Detected %i circles\n", circles.size());
	// Draw the circles detected
	
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(sourceImage, center, 3, Scalar(0, 255, 0), -5, 8, 0);
		// circle outline
		circle(sourceImage, center, radius, Scalar(0, 0, 255), 6, 8, 0);
	}
	/// Show your results
	imshow("Hough Circle Transform Demo", sourceImage);

	waitKey(20);
	return circles.size() > 0 ? true : false;
}

Mat ImageProcessor::applySobelDerivative(Mat sourceImg) {

	
	// Declare & initialise variables
	Mat sourceImg_grey;
	Mat gradient;
	Mat gradientX, gradientY;
	Mat absGradientX, absGradientY;
	imshow(rawWindow, sourceImg);
	int ddepth = CV_16S;
	double scale = 1;
	double delta = 0;

	if (!sourceImg.data)
	{
		throw "No source data!\n";
	};

	GaussianBlur(sourceImg, sourceImg_grey, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
	
	//Convert to grey
	cvtColor(sourceImg, sourceImg_grey, CV_BGR2GRAY);
	//Sobel: Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator
	//Parameters: void Sobel(InputArray src, OutputArray dst, int ddepth, int dx, int dy, int ksize=3, double scale=1, double delta=0, int borderType=BORDER_DEFAULT )
	//calculate derivatives using sobel operator
	//gradient X
	Sobel(sourceImg_grey, gradientX, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	convertScaleAbs(gradientX, absGradientX);
	//gradient Y
	Sobel(sourceImg_grey, gradientY, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

	//convertScaleAbs: Scales, computes absolute values and converts the result to 8-bit
	//Parameters: (Source array, destination array)
	
	convertScaleAbs(gradientY, absGradientY);

	//calc approx total gradient
	addWeighted(absGradientX, 0.5, absGradientY, 0.5, 0, gradient);
	imshow(sobelWindow, gradient);
	cv::waitKey(20);
	return gradient;
}

void ImageProcessor::faceDetection(Mat frame)
{
	
	vector<Rect> faces;
	String faceMarker_name = "H:/IR_Coursework/IR_Coursework/haarcascades/haarcascade_frontalface_alt";
	String eyesMarker_name = "H:/IR_Coursework/IR_Coursework/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier faceMarker;
	CascadeClassifier eyesMarker;

	//Load the .xml files
	if(!faceMarker.load(faceMarker_name))
	{
		throw "No source data!\n";
	}
	if (!eyesMarker.load(eyesMarker_name))
	{
		throw "No source data!\n";
	}

	//detect faces
	//detectMultiScale parameters: (const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())
	faceMarker.detectMultiScale(frame, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		//parameters: (x,y) finding half the width of x coordinate, finding half height of y coordinate - find center point
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		
		//ellipse paramerters: (Mat& img, Point center, Size axes, double angle, double startAngle, double endAngle, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 204, 204), 2, 8, 0);

		Mat faceROI = frame(faces[i]);
		vector<Rect> eyes;
		//detect eyes in individual face
		eyesMarker.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		for (size_t k = 0; k < eyes.size(); k++)
		{
			Point center(faces[i].x + eyes[k].x + eyes[k].width*0.5, faces[i].y + eyes[k].x + eyes[k].height*0.5);
			int radius = cvRound(0.25*(eyes[k].width + eyes[k].height));

			// circle parameters: (Mat& img, Point center, int radius, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
			circle(frame, center, radius, Scalar(234, 205, 255), 2, 8, 0);
		}

	}
	imshow(detectFace, frame);
}