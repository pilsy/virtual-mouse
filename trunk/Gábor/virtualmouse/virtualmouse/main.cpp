#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>

#include "Motion.h"
#include "Click.h"

static int width = 320, height = 240;
//int xMin = 200, yMin = 200;				// Minimumhelyek koordinátái
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
	CvSize size = cvSize( width, height);
	int key=0;

	motion->GetDesktopResolution(horizontal, vertical);
 
    /* initialize camera */
    capture = cvCaptureFromCAM( 1 );

	cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,30);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,width);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,height);
 
    /* sikerült-e megnyitni a camot */
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return 1;
    }
 
    cvNamedWindow( "Original", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Threshold", CV_WINDOW_AUTOSIZE );

	    grey  = cvCreateImage(size, IPL_DEPTH_8U, 1);
		edges = cvCreateImage(size, IPL_DEPTH_8U, 1);
		segment = cvCreateImage(size, IPL_DEPTH_8U, 1);
 
    while( key != 27 ) {
       
		// aktuális frame
        frame = cvQueryFrame( capture );
 
        if( !frame ) break;


		cvCvtColor(frame, grey, CV_BGR2GRAY);
		cvSmooth(grey, grey, CV_GAUSSIAN, 11, 11, 2, 0);
		cvThreshold(grey, segment, 140, 255, 0);
		//cvThreshold(grey, segment, 0, 255, CV_THRESH_OTSU);

		motion->Hotkey(key);

		if (motion->stop == 0){

			motion->FindCircle(segment,storage);
		}

		if (motion->stop == 1){

			motion->CutRegion(segment);

		}

		cvCanny( segment, edges, 60, 150, 3);

		motion->getMin(segment);

		if (motion->startMove){

			motion->MoveTheMouse();
		}

		/*
		kereszt kirajzolása
		*/
		motion->DrawKereszt(frame);

		/*-------------------------------------------------ujjmozgas------------------------------------------------*/
		
		click->ConvexBurok(segment);
		click->FindFingers(frame);

		click->Hotkey(key);

		/*
		for ( int i = 0; i < height; i++ ){

				for ( int j = 0; j < width; j++ ){

					value = cvGet2D(edges,i,j);

					if (value.val[0] == 0){
						value.val[0] = 255;
						cvSet2D(edges, i,j,value);
					} else {
						value.val[0] = 0;
						cvSet2D(edges, i,j,value);
					}

				}

		}

		//cvDistTransform(edges, edges, CV_DIST_L2, 3, 0,0);
		
		cvDistTransform(edges, edges, CV_DIST_L1, 5);
		*/

        /* display current frame */
        //cvShowImage( "Canny", edges );
		//cvShowImage( "Grey", grey );
		cvShowImage( "Original", frame );
		cvShowImage( "Threshold", segment );
 
        /* exit if user press 'q' */
        key = cvWaitKey( 1 );
    }
 
	

    /* free memory */
	cvReleaseCapture( &capture );
    cvDestroyAllWindows();

	delete motion;
	delete click;
 
    return 0;
}