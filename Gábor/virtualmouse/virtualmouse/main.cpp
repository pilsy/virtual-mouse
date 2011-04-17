#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>

#include "Motion.h"
#include "Click.h"

using namespace std;

static int width = 320, height = 240;
int horizontal = 0, vertical = 0;

CvMemStorage* storage = cvCreateMemStorage(0);
CvMemStorage* storage2 = cvCreateMemStorage(0);

Motion *motion=new Motion(width,height);
Click *click=new Click(storage2);

void T1(int id){
	id;
}


int main( int argc, char **argv )
{
	IplImage  *frame = NULL;
	CvCapture *capture = 0;
	IplImage  *segment=NULL;
	IplImage  *grey=NULL;
	IplImage  *edges=NULL;
	IplImage *hsvImg, *hue, *sat, *val;
	IplImage  *hsv_mask2=NULL;
	IplImage  *hsv_mask=NULL;
	IplImage* imageSkinPixels;
	CvSize size = cvSize( width, height);
	int key=0;
	CvScalar value;
	CvScalar value2;
	float r,g;
	int S1 = 50, S2 = 180, V1 = 120, V2 = 230;
	//CvScalar  hsv_min = cvScalar(0, 30, 80, 0);
	//CvScalar  hsv_max = cvScalar(20, 150, 255, 0);

	motion->GetDesktopResolution(motion->horizontal, motion->vertical);
 

    capture = cvCaptureFromCAM( 1 );

	cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,30);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,width);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,height);

    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return 1;
    }
 
    cvNamedWindow( "Original", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Segment", CV_WINDOW_AUTOSIZE );

	cvCreateTrackbar("Smin", "Segment", &S1, 100, T1);
	cvCreateTrackbar("Smax", "Segment", &S2, 250, T1);
	cvCreateTrackbar("Vmin", "Segment", &V1, 180, T1);
	cvCreateTrackbar("Vmax", "Segment", &V2, 255, T1);

	grey  = cvCreateImage(size, IPL_DEPTH_8U, 1);
	edges = cvCreateImage(size, IPL_DEPTH_8U, 1);
	segment = cvCreateImage(size, IPL_DEPTH_8U, 1);
	hsvImg = cvCreateImage(size, IPL_DEPTH_8U, 3);
	hsv_mask = cvCreateImage(size, IPL_DEPTH_8U, 1);
	imageSkinPixels = cvCreateImage(size, IPL_DEPTH_8U, 1);

	hue = cvCreateImage(size, IPL_DEPTH_8U, 1);
    sat = cvCreateImage(size, IPL_DEPTH_8U, 1);
    val = cvCreateImage(size, IPL_DEPTH_8U, 1);
 
    while( key != 27 ) {
       
		// aktuális frame
        frame = cvQueryFrame( capture );
 
        if( !frame ) break;


		cvCvtColor(frame, grey, CV_BGR2GRAY);
		//cvSmooth(grey, segment, CV_GAUSSIAN, 11, 11, 4, 2);
		//cvThreshold(grey, segment, 0, 255, CV_THRESH_OTSU);


		if (click->segment){

			CvScalar  hsv_min = cvScalar(0, S1, V1, 0);
			CvScalar  hsv_max = cvScalar(24, S2, V2, 0);

			cvCvtColor(frame, hsvImg, CV_BGR2HSV);
			cvSplit(hsvImg, hue, sat, val, 0);

			cvInRangeS (hsvImg, hsv_min, hsv_max, hsv_mask);

			cvDilate(hsv_mask, hsv_mask, NULL, 5);
			cvSmooth( hsv_mask, hsv_mask, CV_MEDIAN, 27, 0, 0, 0 );

			//cvCanny(hsv_mask, edges, 1, 3, 5);

			/*
			cvCvtColor(frame, hsvImg, CV_BGR2HSV);
			cvSplit(hsvImg, hue, sat, val, 0);

			cvThreshold(hue, hue, 36, 255, CV_THRESH_BINARY_INV);
			cvThreshold(sat, sat, 50, 255, CV_THRESH_BINARY);
			cvThreshold(val, val, 60, UCHAR_MAX, CV_THRESH_BINARY);

			cvAnd(hue, sat, imageSkinPixels);
			cvAnd(imageSkinPixels, val, imageSkinPixels);

			cvSmooth(imageSkinPixels, imageSkinPixels, CV_GAUSSIAN, 11, 11, 2, 1);
			cvThreshold(imageSkinPixels, imageSkinPixels, 80, 255, CV_THRESH_BINARY);
			*/
		} else {

			for (int i = 0; i < height; i++){
				for (int j = 0; j < width; j++){

					value = cvGet2D(frame,i,j);

					r = value.val[2] / (value.val[0] + value.val[1] + value.val[2]);
					g = value.val[1] / (value.val[0] + value.val[1] + value.val[2]);

					if (r > 0.35 && r < 0.55 && g > 0.26 && g < 0.4){

						value.val[0] = 255;

						cvSet2D(hsv_mask, i, j, value);
					} else {

						value.val[0] = 0;

						cvSet2D(hsv_mask, i, j, value);
					}

				}
			}

			cvDilate(hsv_mask, hsv_mask, NULL, 4);
			cvErode(hsv_mask, hsv_mask, NULL, 3);
		}


		cvShowImage("Segment", hsv_mask);

		motion->Hotkey(key);

		
			if (motion->startMove){
				click->Clicking();
				motion->getMin(click->fingerTip2);
				motion->MoveTheMouse();
			}

	
			//motion->DrawKereszt(frame);
		
			click->ConvexBurok(hsv_mask, frame);
			click->FindFingers(frame);

		

		click->Hotkey(key);

		cvShowImage("Original", frame );
		//cvShowImage( "h", hue );
		//cvShowImage( "s", sat );
		//cvShowImage( "v", val );

        key = cvWaitKey( 1 );
    }
 
	

	cvReleaseCapture( &capture );
    cvDestroyAllWindows();

	delete motion;
	delete click;
 
    return 0;
}
