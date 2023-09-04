#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgHSV, mask;
int hmin = 0, smin = 110, vmin = 153;
int hmax = 19, smax = 240, vmax = 255;

//////////////////// Color Detection ///////////////////
void main() {
	std::string path = "Resources/lambo.png";
	cv::Mat img = cv::imread(path);


	// use this to find out what Hue, Sat, Val values are needed to detect a certain color
	// NOTE: must change the waitKey arg from 0 to 1 and use a while(true) loop
	namedWindow("Trackbars", (640, 200));
	createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	createTrackbar("Sat Min", "Trackbars", &smin, 255);
	createTrackbar("Sat Max", "Trackbars", &smax, 255);
	createTrackbar("Val Min", "Trackbars", &vmin, 255);
	createTrackbar("Val Max", "Trackbars", &vmax, 255);

	cv::VideoCapture cap(0);
	while (true) {
		cap.read(img);
		// convert to HSV to make the colors pop out more, easier to be picked
		cvtColor(img, imgHSV, COLOR_BGR2HSV);

		Scalar lower(hmin, smin, vmin);
		Scalar upper(hmax, smax, vmax);
		inRange(imgHSV, lower, upper, mask);

		cv::imshow("Image", img);
		cv::imshow("ImageHSV", imgHSV);
		cv::imshow("Image Mask", mask);
		cv::waitKey(1);
		cout << hmin << ", " 
			<< smin << ", " 
			<< vmin << ", " 
			<< hmax << ", " 
			<< smax << ", " 
			<< vmax << endl;
	}

}
