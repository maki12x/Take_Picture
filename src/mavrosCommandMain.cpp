#include <iostream>
#include "ros/ros.h"
#include "mavrosCommand.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

int frequency = 20;
int loopCounter = 0;


const int alpha_slider_max = 255;
int alpha_slider;
double alpha;
double beta;
Mat image_open;
Mat frame;
Mat lower_red_hue_range, upper_red_hue_range, red_hue_image, hsv_image;

void on_trackbar( int, void* )
{
	cvtColor(frame, hsv_image, COLOR_BGR2HSV);
	
	inRange(hsv_image, Scalar(0,100,90), Scalar(30,255,255) , lower_red_hue_range);
	inRange(hsv_image, Scalar(140,100,90), Scalar(179,255,255) , upper_red_hue_range);
	
	addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
	GaussianBlur(red_hue_image, red_hue_image, Size(9,9), 2, 2);
	
	int morph_size = 1;
	Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
	morphologyEx(red_hue_image, image_open, MORPH_OPEN, element, Point(-1,-1), 3 ); 
	
	vector<Vec3f> circles;

    HoughCircles( image_open, circles, CV_HOUGH_GRADIENT,1.2,image_open.rows/8,200, 50,0, 0 );
	
	cvtColor(image_open, image_open, COLOR_GRAY2BGR);
	
   // Draw the circles detected
    for( size_t i = 0; i < circles.size(); i++ )
    {
         Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
         int radius = cvRound(circles[i][2]);
         // circle center
         circle( image_open, center,3, Scalar(0,0,255), -1, 8, 0 );
         // circle outline
         circle( image_open, center, radius, Scalar(0,0,255),5, 8, 0 );
         cout << "Circle " << i << " center: " << center << " radius: " << radius << endl;
    }
	
	imshow( "okno 1", image_open );
}

int main(int argc, char* argv[]){

	ros::init(argc, argv, "take_picture");
	mavrosCommand command;
	
	ros::Rate loop_rate(frequency);
	sleep(1);
		
	VideoCapture cap(1);
	
	if (cap.isOpened() == false)  
	{
		cout << "Cannot open the video camera" << endl;
		return -1;
	} 
	
	namedWindow( "okno 1", WINDOW_AUTOSIZE );
	alpha_slider = 0;
	
	
	char TrackbarName[50];
	sprintf( TrackbarName, "Alpha x %d", alpha_slider_max );
	
	createTrackbar( TrackbarName, "okno 1", &alpha_slider, alpha_slider_max, on_trackbar );
	
	while (ros::ok()) 
	{
		
		cap.read(frame);
		
		on_trackbar( alpha_slider, 0 );
		
		waitKey(10);
		
		ros::spinOnce();
		loop_rate.sleep();
	}	
	cap.release();
	return 0;
}
