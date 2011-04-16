#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>

#include "Motion.h"
#include "Click.h"

using namespace std;

static int width = 640, height = 480;
int horizontal = 0, vertical = 0;

CvMemStorage* storage = cvCreateMemStorage(0);
CvMemStorage* storage2 = cvCreateMemStorage(0);

Motion *motion=new Motion(width,height);
Click *click=new Click(storage2);


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

	motion->GetDesktopResolution(motion->horizontal, motion->vertical);
 

    capture = cvCaptureFromCAM( 1 );

	cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,10);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,width);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,height);

    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return 1;
    }
 
    cvNamedWindow( "Original", CV_WINDOW_AUTOSIZE );
	//cvNamedWindow( "Threshold", CV_WINDOW_AUTOSIZE );

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
		cvSmooth(grey, segment, CV_GAUSSIAN, 11, 11, 4, 2);
		//cvThreshold(grey, segment, 0, 255, CV_THRESH_OTSU);

		/*
		cvCvtColor(frame, HSV, CV_RGB2HSV);
		
		cvSplit(HSV, hue, sat, val, 0);



		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){

				value = cvGet2D(hue,i,j);
				value2 = cvGet2D(val,i,j);

				//cout << "1(" << value.val[0] << ")" << endl;

				if (value.val[0] > 30 && value.val[0] < 40 && value2.val[0] > 120){

					value.val[0] = 255;
					cvSet2D(hsv_mask, i, j, value);
				} else {
					value.val[0] = 0;
					cvSet2D(hsv_mask, i, j, value);
				}

			}
		}
		

		cvCvtColor(frame, hsvImg, CV_BGR2HSV);

		cvSplit(hsvImg, hue, sat, val, 0);


		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){

				value = cvGet2D(hue,i,j);

				if (value.val[0] > 0 && value.val[0] < 50){

					value.val[0] = 255;

					cvSet2D(hue, i, j, value);
				} else {

					value.val[0] = 0;

					cvSet2D(hue, i, j, value);
				}


				value = cvGet2D(sat,i,j);

				if (value.val[0] > 23 && value.val[0] < 60){

					value.val[0] = 255;
					cvSet2D(sat, i, j, value);
				} else {

					value.val[0] = 0;
					cvSet2D(sat, i, j, value);
				}
			}
		}

		//cvThreshold(hue, hue, 40, 255, CV_THRESH_BINARY_INV);
		//cvThreshold(hue, hue, 10, 255, CV_THRESH_BINARY);

		//cvThreshold(sat, sat, 50, 255, CV_THRESH_BINARY);

		cvAnd(hue, sat, imageSkinPixels);
		cvAnd(imageSkinPixels, val, imageSkinPixels);

		cvSmooth(imageSkinPixels, imageSkinPixels, CV_GAUSSIAN, 11, 11, 2, 0);
		cvThreshold(imageSkinPixels, imageSkinPixels, 64, 255, CV_THRESH_BINARY);

		cvShowImage("Skin Pixels", imageSkinPixels);
		*/

		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){

				value = cvGet2D(frame,i,j);

				r = value.val[2] / (value.val[0] + value.val[1] + value.val[2]);
				g = value.val[1] / (value.val[0] + value.val[1] + value.val[2]);

				if (r > 0.39 && r < 0.5 && g > 0.28 && g < 0.38){

					value.val[0] = 255;

					cvSet2D(imageSkinPixels, i, j, value);
				} else {

					value.val[0] = 0;

					cvSet2D(imageSkinPixels, i, j, value);
				}

			}
		}

		cvDilate(imageSkinPixels, imageSkinPixels, NULL, 3);
		cvErode(imageSkinPixels, imageSkinPixels, NULL, 1);

		cvSmooth(imageSkinPixels, imageSkinPixels, CV_MEDIAN, 7, 7);

		cvShowImage("Skin Pixels", imageSkinPixels);

		motion->Hotkey(key);


			if (motion->startMove){

				motion->MoveTheMouse();
			}

	
			//motion->DrawKereszt(frame);
		
			click->ConvexBurok(imageSkinPixels, frame);
			click->FindFingers(frame);

		

		click->Hotkey(key);

		cvShowImage( "Original", frame );
		//cvShowImage( "Threshold", segment );

        key = cvWaitKey( 1 );
    }
 
	

	cvReleaseCapture( &capture );
    cvDestroyAllWindows();

	delete motion;
	delete click;
 
    return 0;
}
