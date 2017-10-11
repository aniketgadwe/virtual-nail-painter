


#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <list>
#include "handfinder.hpp"
#include <stdio.h>
#include <unistd.h>	

using namespace cv;
using namespace std;

void print_help(){
    cout << "Virtual Nail Painter demo.\n" << \
            "Default mode is paint mode.\n" << \
            "   [*] press 'p' to toggle paint mode.\n" << \
            "   [*] press 't' to toggle track mode.\n" << \
            "   [*] press ESC to exit.\n" << endl;
}


int main(int argc, const char * argv[])
 {
 print_help();
 Mat template_image=imread("/home/aniket/Ravi_Raj_Reddy/Nail _painter/blue.png");
 Mat template1 = imread("/home/aniket/Ravi_Raj_Reddy/Nail _painter/pink.jpeg");
if (template1.cols == 0) {
     	cout << "Error reading file "<< endl;
     	return -1;
	} 
 HandFinder hand_finder("palm.xml");
   

	if (template_image.empty()){
            cout << "Error : Image cannot be loaded..!!" << endl;
            return -1;
            }
  Mat hand_cursor = imread("/home/aniket/Ravi_Raj_Reddy/Nail _painter/palm.png");
     	
	if (hand_cursor.cols == 0) {
     	cout << "Error reading file "<< endl;
     	return -1;
	} 
		

		
 VideoCapture cam(0);
    	if(!cam.isOpened()){
        cout<<"ERROR not opened "<< endl;
        return -1;
    	}
   
	
	Mat img;
        Mat clone_img;
        Mat paint_clone_img;
    	Mat mask_img;
    	Mat img_threshold;
    	Mat thresh;
    	Mat img_gray;
    	Mat img_roi;
    	Mat mask_img_roi;
    	Mat mask_img_roi_copy;
        Mat mask_paint_clone_img;
    	Mat img_face;
    	Mat only_face;
    	Mat fore;
    	Mat back;
    	Mat mask;
    	Mat nail;
    	Mat mask_mask;
    	Mat template_mask;
        Mat temp_clone_img;
        int area_hand=0;
    	int count =0;       
    	int flag=0;
    	int lowThreshold=200;
   	int ratio=5;
  	int kernel_size=5;
       
        bool running=true;
        bool paint =true;
        bool track =false;
    	list<cv::Point2f> hand_npoints;           // normalized hand points
	
     
   BackgroundSubtractorMOG2 bg;
        bg.set("history",1000);
	bg.set("nmixtures",5);
	bg.set("detectShadows",true);
       
   int backgroundFrame=500;
        
  
    while(running){
        
	bool b_success=cam.read(img);
        if(!b_success){
            cout<<"ERROR : cannot read"<<endl;
            return -1;
        }

        cv::flip(img, img, 1);
          
        
        img.copyTo(clone_img); 
        img.copyTo(mask_img);
        
      //  if(!track)
      //       hand_cursor.copyTo(img(Rect(500,400,30,30)));
       
        if(track){
	cv::Rect hand_rect = hand_finder.find(clone_img);
        cv::Point2f hand_pos(hand_rect.x+hand_rect.width/2, hand_rect.y+hand_rect.height/2);
        resize(hand_cursor,hand_cursor,Size(30,30)); 
        resize(template1,template1,Size(50,50));
        
       
        
        if(hand_rect.width * hand_rect.height){
            
            hand_cursor.copyTo(img(Rect(int(hand_pos.x -15),int(hand_pos.y-15),30,30)));
              template1.copyTo(img(Rect(100,150,50,50)));
	    // normalize hand position
            //hand_pos = cv::Point2f((float)hand_pos.x/(float)img.cols, (float)hand_pos.y/(float)img.rows);
           
             
        }
	
	else{
            // no hands found
            hand_pos = cv::Point2f(-1.0f,-1.0f);
            hand_cursor.copyTo(img(Rect(100,400,30,30)));
             template1.copyTo(img(Rect(100,150,50,50)));
        }
         mask_paint_clone_img.copyTo(img(Rect(330,90,250,250)));
         area_hand=hand_rect.width * hand_rect.height;
         
         if(hand_pos.x >100 && hand_pos.x <150 && hand_pos.y >150  && hand_pos.y <200 && area_hand <40000 )
          {
          template_image=template1;
         
         
          flag=1;   
          }
          
        imshow("img",img);
	
       }

         
     
	
       
	Rect roi(340,100,230,230);
       	Rect roi1(330,90,250,250);
       	rectangle(clone_img,roi1,Scalar(255,0,0),2,8,0);
       	rectangle(mask_img,roi1,Scalar(255,0,0),2,8,0);
          
        if(paint || flag==1){
      
         if(flag==1){
           temp_clone_img.copyTo(clone_img);
 	   temp_clone_img.copyTo(mask_img);}
        else 
	    clone_img.copyTo(temp_clone_img);
        

	Mat template_roi=template_image(Rect(0,0,40,40));
       
        img_roi=clone_img(roi);
        
        mask_img_roi= mask_img(roi);
       	Mat temp_img_roi=clone_img(roi);
      
        temp_img_roi.copyTo(template_mask);
        mask_img_roi.copyTo(mask_img_roi_copy);
         
	Mat grayed,canny;

	vector<Mat> rgbChannels(3);
	split(img_roi, rgbChannels);

	Mat g,new_rgb,diff_red,diff_blue,diff_green,gray_diff,threshold_canny,gray;
        g = Mat::ones(Size(img_roi.cols, img_roi.rows), CV_8UC1);

	diff_red=(rgbChannels[2]-rgbChannels[1])+g;
	diff_green=(rgbChannels[1])+g;
	diff_blue=(rgbChannels[0])+g;       
	
	vector<Mat> channels;
	channels.push_back(diff_blue);
	channels.push_back(diff_green);
	channels.push_back(diff_red);
	merge(channels, new_rgb);

       	Mat canny_gray;
     	
       	cvtColor(img_roi ,canny_gray,CV_BGR2GRAY);
        cvtColor(new_rgb ,grayed,CV_BGR2GRAY);
        Canny(canny_gray,canny, lowThreshold, lowThreshold*ratio, kernel_size );         
        threshold(grayed,thresh,50,255,(CV_THRESH_BINARY_INV+CV_THRESH_OTSU));         
        threshold(canny_gray,grayed,80,255,(CV_THRESH_BINARY_INV+CV_THRESH_OTSU));  
        

        dilate(canny,canny, getStructuringElement(MORPH_ELLIPSE,Size(5, 5)) );
        erode(canny,canny, getStructuringElement(MORPH_ELLIPSE, Size(5,5)) );  
          
        dilate(canny,canny, getStructuringElement(MORPH_ELLIPSE,Size(5, 5)) );
        erode(canny,canny, getStructuringElement(MORPH_ELLIPSE, Size(3,3)) );  
                   
        bitwise_or(canny,grayed,grayed);
       
        Mat frame,mask,background ;
        img_roi.copyTo(frame);
         
       if(backgroundFrame>0)
		{bg.operator ()(frame,fore,-1);backgroundFrame--;}
		else
                 {
		bg.operator()(frame,fore,0);
                if(backgroundFrame==1)
			cout<<" done "<<endl;		
		}
                 

		//Get backgroundimage to display it
		 GaussianBlur(fore, fore, Size(5,5),2.5,2.5);
                 threshold(fore, fore, 45,255,THRESH_BINARY);
		//Enhance edges in the foreground by applying erosion and dilation
	        dilate(fore,fore, getStructuringElement(MORPH_ELLIPSE,Size(3, 3)) );
            	erode(fore,fore, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );  
                imshow("fore",fore);
                
                bitwise_or(grayed,fore,fore);
              
    		Mat fore_img; 
       		fore_img = Scalar::all(0);
     		frame.copyTo(fore_img, fore);
     		
                cvtColor(fore_img,img_gray,CV_BGR2GRAY);
               
                 fore.copyTo(thresh);
		 fore.copyTo(img_threshold);
                 Mat temp = fore;
                 temp.copyTo(mask);
               
  		     
         Mat drawing_and= Mat::zeros(mask.size(),CV_8UC3);
         Mat drawing = Mat::zeros(mask.size(),CV_8UC3);
         Mat img_roi_roi = Mat::zeros(mask.size(),CV_8UC3) ;   
         Mat dst1 = Mat::zeros(mask.size(),CV_8UC1);       
         Mat dst2  = Mat::zeros(mask.size(),CV_8UC3);
     	 Mat dst3  = Mat::zeros(mask.size(),CV_8UC3);
         Mat dst1_and_not=Mat::zeros(mask.size(),CV_8UC1);
         Mat dst1_and =Mat::zeros(mask.size(),CV_8UC1);
         Mat drawing_and_and = Mat::zeros(mask.size(),CV_8UC3);
    	 Mat result  = Mat::zeros(mask.size(),CV_8UC3);
         
       
         Mat temp_mask_mask =Mat::zeros(mask.size(),CV_8UC3); 
         Mat dst1_mask =Mat::zeros(mask.size(),CV_8UC1); 
         vector<vector<Point> >contours;
         vector<Vec4i>hierarchy;
           
         findContours(img_threshold,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,Point());
        
       
	if(contours.size()<=0)
                 continue;
        if(contours.size()>0){
                size_t indexOfBiggestContour = -1;
	            size_t sizeOfBiggestContour = 0;

	            		for (size_t i = 0; i < contours.size(); i++){
		           		if(contours[i].size() > sizeOfBiggestContour){
			       		sizeOfBiggestContour = contours[i].size();
			       		indexOfBiggestContour = i;
		          		}
                		}
               
		vector<vector<int> >hull(contours.size());
                vector<vector<Point> >hullPoint(contours.size());
                vector<vector<Vec4i> >defects(contours.size());
                vector<vector<Point> >defectPoint(contours.size());
                vector<vector<Point> >contours_poly(contours.size());
                Point2f rect_point[4];
                vector<RotatedRect>minRect(contours.size());
                vector<Rect> boundRect(contours.size());
                
		for(size_t i=0;i<contours.size();i++){
                    if(contourArea(contours[i])>5000){
                        convexHull(contours[i],hull[i],true);
                        convexityDefects(contours[i],hull[i],defects[i]);
                        	if(indexOfBiggestContour==i){
                          	  minRect[i]=minAreaRect(contours[i]);
                            		for(size_t k=0;k<hull[i].size();k++){
                                	int ind=hull[i][k];
                                	hullPoint[i].push_back(contours[i][ind]);
                            		}
                            		count =0;
                                       
                            		for(size_t k=0;k<defects[i].size();k++){
                                		if(defects[i][k][3]>13*256){
                                     		//int p_start=defects[i][k][0];   
                                    		int p_end=defects[i][k][1];
                                               
                                    		int p_far=defects[i][k][2];
                                    		defectPoint[i].push_back(contours[i][p_far]);

				    
                                   		template_roi.copyTo(mask_img(Rect((contours[i][p_end].x-20 + 340),(contours[i][p_end].y-20+100),40,40)));
                                   
                                   		img_roi_roi=mask_img(roi);
                                                                  
                                    		circle(drawing,contours[i][p_end],18,Scalar(255,255,255),-1);
                                   		circle(dst1,contours[i][p_end],18,Scalar(255,255,255),-1);
                                              
                                               
                                    		drawContours(temp_mask_mask, contours, i,Scalar(255,255,255),-1, 8, vector<Vec4i>(), 0, Point() ); 
                                      		drawContours(dst1_mask, contours, i,Scalar(255,255,255),-1, 8, vector<Vec4i>(), 0, Point() );  
                                           count++;
                                		}

		                            }
                                	        
                                        	bitwise_and(drawing,temp_mask_mask,drawing_and);
                                                
                                      		bitwise_and(img_roi_roi,drawing_and,drawing_and_and);
                                                
                                      		bitwise_and(dst1,dst1_mask,dst1_and);                                          
						drawing_and_and.copyTo(dst2,dst1_and);                                      
						bitwise_not(dst1_and,dst1_and_not);
						mask_img_roi_copy.copyTo(dst3,dst1_and_not);
                                      		result=dst2 + dst3;
                               			result.copyTo(clone_img(roi));
                                                
                            			approxPolyDP(contours[i],contours_poly[i],3,false);
                            			boundRect[i]=boundingRect(contours_poly[i]);
                            
                            			minRect[i].points(rect_point);
                                                

                        	}
                    	}

                }
                
               
               paint_clone_img=clone_img(roi1);
               paint_clone_img.copyTo(mask_paint_clone_img); 
               
               flag=0;
             
              imshow("img",clone_img);
              
                                                                              
           
             }

        }
  
      
	 

                 switch(255 & cv::waitKey(30)){
            case 27:    //exit
                running = false;
                break;
            case 't':
                track = !track;
                cout << "track mode: " << track << endl;
                break;
            case 'p':
                paint = !paint;
                cout << "paint mode: " << paint << endl;
                break;
        
         
             default:
                break;
        }

    }
   

     return 0;
}

