#include <iostream>
#include "ros/ros.h"
#include "mavrosCommand.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;


double pictureFrequency = 40; //40 = 1 photo per  two seconds

int frequency = 20;
int loopCounter = 0;




int main(int argc, char* argv[]){

	ros::init(argc, argv, "take_picture");
	mavrosCommand command;
	
	ros::Rate loop_rate(frequency);
	sleep(1);
		
	VideoCapture cap(0);
	
	if (cap.isOpened() == false)  
	{
		cout << "Cannot open the video camera" << endl;
		return -1;
	} 
	
	int cntr = 0;
	int rcValue = 0;
	string name = get_username();
	
	while (ros::ok()) 
	{
		Mat frame;
		
		bool bSuccess = cap.read(frame);
		if (bSuccess == false) 
		{
			cout << "Video camera is disconnected" << endl;
		}
		
		rcValue = command.getRCInput();
		
		if(loopCounter >= pictureFrequency & rcValue >= 1500)
		{
			string savingName = "/home/" + name + "/zdj/" + to_string(cntr) + ".jpg";
			imwrite(savingName, frame);
			cout << "PICTURE: " << cntr << " TAKEN" << endl;
			
			Mat bwFrame;
			
			cvtColor(frame, bwFrame, COLOR_BGR2HSV);
			Mat lower_red_hue_range;
			Mat upper_red_hue_range;
			inRange(bwFrame, Scalar(0,100,100), Scalar(10,255,255) , lower_red_hue_range);
			inRange(bwFrame, Scalar(160,100,100), Scalar(179,255,255) , upper_red_hue_range);
			Mat red_hue_image;
			addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
			GaussianBlur(red_hue_image, red_hue_image, Size(9,9), 2, 2);
			savingName = "/home/" + name + "/zdj2/" + to_string(cntr) + ".jpg";
			imwrite(savingName, red_hue_image);
			cntr++;
			loopCounter = 0;
		}
		
		loopCounter++;
		ros::spinOnce();
		loop_rate.sleep();
	}	
	cap.release();
	return 0;
}
