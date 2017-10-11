//
//  main.cpp
//  simple_hand_tracking
//
//  Created by Minghuang Ma on 9/17/14.
//  Copyright (c) 2014 Minghuang Ma. All rights reserved.
//

#include <iostream>
#include <vector>
#include <list>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "handfinder.hpp"
#include <stdio.h>

using namespace std;
using namespace cv;


int main(int argc, const char * argv[])
{
    
    HandFinder hand_finder("palm.xml");
   
    
    // setup camera
    cv::VideoCapture cap;
    if(!cap.open(0)){
        cout << "failed to open camera 0..." << endl;
        return -1;
    }
    
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
 	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    	cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 0);
 
   
    
    	vector<vector<cv::Point> > contours;
   
    	cv::Mat raw_img;
    	cv::Mat  hand_img;
    	cv::Mat fg_mask;
    	cv::Mat ui_img;
    	cv::Mat pic = cv::Mat::zeros(480, 640, CV_8UC1);
   
    // normalized hand points
    	list<cv::Point2f> hand_npoints;
    	   
    	int template_id = 0;
    	float score_threshold = 0.8f;
    	int count_down_count = 0;
    	bool debug = false;
     
	Mat hand_cursor = imread("/home/aniket/simple-hand-tracking-master/simple_hand_tracking/palm.png");
     	
	if (hand_cursor.cols == 0) {
     	cout << "Error reading file "<< endl;
     	return -1;
	}
	
    while(1){
        
	if(!cap.read(raw_img)){
            cout << "failed to read from camera..." << endl;
            return -1;
        }


        cv::flip(raw_img, raw_img, 1);
      
        ui_img = raw_img.clone();
       
        // find hand
        cv::Rect hand_rect = hand_finder.find(raw_img);
        cv::Point2f hand_pos(hand_rect.x+hand_rect.width/2, hand_rect.y+hand_rect.height/2);
        resize(hand_cursor,hand_cursor,Size(30,30)); 
 
        if(hand_rect.width * hand_rect.height){
            // hand found, draw bounding rect
            if(debug) cv::rectangle(ui_img, hand_rect.tl(), hand_rect.br(), cv::Scalar(0,255,0));
          
            hand_cursor.copyTo(ui_img(Rect(int(hand_pos.x -15),int(hand_pos.y-15),30,30)));
	    // normalize hand position
            hand_pos = cv::Point2f((float)hand_pos.x/(float)raw_img.cols, (float)hand_pos.y/(float)raw_img.rows);
             
        }else{
            // no hands found
            hand_pos = cv::Point2f(-1.0f,-1.0f);
            hand_cursor.copyTo(ui_img(Rect(500,400,30,30)));
            
        }
        
        if(waitKey(3)==27)
            break;
	 imshow("ui", ui_img);
    
        
    }
    return 0;
}

