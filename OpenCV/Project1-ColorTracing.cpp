/* NOTE: better camera will yield better results, this was done on a laptop with built-in cam so it's not as good
This program uses opencv to track objects of colors and display their path across the screen as colored dots
The color to be tracked are set using a set of filters of hue, saturation and value (HSV values)
To find the filter for the correct color, we must use the ColorPicker.cpp to find the right HSV values
Once the filter values are found, save the values to the vector<vector<int>> color_filters to be tracked
The color to be output to the screen are BGR values and are set in the vector<Scalar> color_values
*/

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;
 //////////////////// Project 1 ///////////////////
cv::Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

Point getContours(Mat imgDil, Mat img) {

	vector<vector< Point >> contours;
	vector<Vec4i> hierarchy;

	// find contours of white region in this binary image
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

	vector<vector<Point>> conPoly(contours.size());	// vector to hold polygon approx of the contours
	vector<Rect> boundRect(contours.size());		// vector to hold rectangular approx of the polygon approx

	Point myPoint{ -1,-1 };
	int max_area = -1;

	for (int i = 0; i < contours.size(); i++) {
		int area = contourArea(contours[i]);
		//cout << area << endl;

		if (area > 1000 && area > max_area) {	// should return point on the region with largest area
			max_area = area;
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);	// approximate figure with a polygon to 0.02 accuracy of 'peri'
			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);		// draw approximated polygon contour

			boundRect[i] = boundingRect(conPoly[i]);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);	// draw bounding rectangle of polygon contour
			imshow("Image", img);
			myPoint = Point(boundRect[i].x, boundRect[i].y);
			//myPoint = Point(boundRect[i].x + (boundRect[i].width / 2), boundRect[i].y);
		}
	}
	return myPoint;
}

vector<string> color_strings{ "Yellow", "Red", "Blue" };

vector<vector<int>> color_filters{
	// hmin, hmax, smin, smax, vmin, vmax
	{17, 66, 210, 39, 250, 255},//yellow
	{151, 116, 29, 179, 245, 255},//red
	{105, 88, 66, 122, 255, 255}, // blue
	// 
	//{0, 177, 0, 64, 220, 255} // white
	//{ 22, 82, 113, 52, 255, 255 },// yellow
	//{ 105, 125, 111, 121, 219, 180 },	// blue
	//{ 133, 160, 123, 179, 255, 255 },	// red
};

vector<Scalar> color_values{
	// BGR
	{0, 255, 255},	// yellow
	{0, 0, 255},	// red
	{255, 0, 0},	// blue

};

vector<Point> findColor(Mat img) {
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	vector<Point> colors(color_filters.size(), Point(-1,-1));

	for (int i = 0; i < color_filters.size(); i++) {
		Scalar lower(color_filters[i][0], color_filters[i][1], color_filters[i][2]);
		Scalar upper(color_filters[i][3], color_filters[i][4], color_filters[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);

		// Preprocessing
		cv::GaussianBlur(mask, imgBlur, cv::Size(3, 3), 3, 0);
		cv::Canny(imgBlur, imgCanny, 50, 150);
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::dilate(imgCanny, imgDil, kernel);
		Point myPoint = getContours(imgDil, img);

		colors[i].x = myPoint.x;
		colors[i].y = myPoint.y;

		//cout << "Point returned: " 
		//	<< color_strings[i] 
		//	<< " \t" 
		//	<< to_string(myPoint.x) 
		//	<< ", " 
		//	<< to_string(myPoint.y) 
		//	<< endl;
	}
	return colors;
}

// draws the points in points_tracing onto image img
void drawOnCanvas(Mat img, vector<vector<Point>> points_tracing, vector<Scalar> color_values) {
	//circle(img, Point(256, 256), 155, Scalar(0, 69, 255), FILLED);
	for (int j = 0; j < points_tracing.size(); j++) {
		vector<Point> points = points_tracing[j];
		for (int i = 0; i < points.size(); i++) {
			if (points[i].x > 0 && points[i].y > 0) {
				circle(img, points[i], 10, color_values[i], FILLED);
			}
		}
	}
}

void check_blackscreen(vector<vector<Point>>& points_tracing) {

}

void main() {
	// // use still images
	//std::string path = "Resources/yellow-red.png";
	//std::string path = "Resources/my_three_colors.png";

	// use web cam
	cv::VideoCapture cap(0);
	cv::Mat img;
	
	vector<vector<Point>> points_tracing;
	while (true) {
		// // still images
		//cv::Mat img = cv::imread(path);
		//cv::imshow("Image", img);
		
		// web cam
		cap.read(img);
		flip(img, img, -1);

		// reset points_tracing if screen is black
		cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);				
		// change the reset factor for reset sensitivity
		int reset_factor = 150;
		threshold(imgGray, imgGray,	reset_factor, 255, THRESH_BINARY);	// convert gray image to binary, only capturing bright values (200 and higher)
		//cv::imshow("ImageGray", imgGray);
		//cout << countNonZero(imgGray) << " / " << imgGray.total() << " = " << (countNonZero(imgGray) / imgGray.total()) << endl;
		if (float(countNonZero(imgGray)) / float(imgGray.total()) < 0.01) {	// if the ratio is < 0.01, reset 
			points_tracing = vector<vector<Point>>();
		}
		//cout << points_tracing.size() << endl;

		points_tracing.push_back(findColor(img));
		drawOnCanvas(img, points_tracing, color_values);

		cv::imshow("Image", img);
		cv::waitKey(1);
	}
}

