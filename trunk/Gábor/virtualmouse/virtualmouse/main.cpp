#include <cv.h>
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
	IplImage* background;
	IplImage* diffimage;
	CvSize size = cvSize( width, height);
	int key=0;
	CvScalar value;
	CvScalar value2;
	float r,g;
	bool hatter = false;
	//int S1 = 38, S2 = 180, V1 = 56, V2 = 230;
	int T = 55;

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

	cvCreateTrackbar("Threshold", "Segment", &T, 255, T1);

	cvCreateTrackbar("Mouse speed", "Segment", &motion->speed, 10, T1);

	grey  = cvCreateImage(size, IPL_DEPTH_8U, 1);
	edges = cvCreateImage(size, IPL_DEPTH_8U, 1);
	segment = cvCreateImage(size, IPL_DEPTH_8U, 1);
	hsvImg = cvCreateImage(size, IPL_DEPTH_8U, 3);
	background = cvCreateImage(size, IPL_DEPTH_8U, 3);
	hsv_mask = cvCreateImage(size, IPL_DEPTH_8U, 1);
	imageSkinPixels = cvCreateImage(size, IPL_DEPTH_8U, 1);
	diffimage = cvCreateImage(size, IPL_DEPTH_8U, 3);

	hue = cvCreateImage(size, IPL_DEPTH_8U, 1);
    sat = cvCreateImage(size, IPL_DEPTH_8U, 1);
    val = cvCreateImage(size, IPL_DEPTH_8U, 1);
 
    while( key != 27 ) {
       
		// aktuális frame
        frame = cvQueryFrame( capture );


		/*Ha lenyomod az "n"- betût átvált a többi 2 szegmentálás közül a HSV-re
		Ha aktív a HSV, akkor az r-re pedig átvált 2D norm színtérre*/
		if (!click->night){
 
			if( !frame ) break;

			switch (key){
			case 'b':
				cvCopy( frame, background, NULL );
				cout << "Background saved!" << endl;
				hatter=true;
			break;
			}

			if (hatter)
				cvAbsDiff(frame,background,diffimage);

			cvCvtColor(diffimage, hsv_mask, CV_BGR2GRAY);
			cvThreshold(hsv_mask, hsv_mask, T, 255 , CV_THRESH_BINARY);
			cvSmooth( hsv_mask, hsv_mask, CV_MEDIAN, 17, 0, 0, 0 );

		} else {

			for (int i = 0; i < height; i++){
				for (int j = 0; j < width; j++){

					value = cvGet2D(frame,i,j);
					r = value.val[2] / (value.val[0] + value.val[1] + value.val[2]);
					g = value.val[1] / (value.val[0] + value.val[1] + value.val[2]);

					if (r > 0.35 && r < 0.55 && g > 0.25 && g < 0.4){

						value.val[0] = 255;

						cvSet2D(hsv_mask, i, j, value);
					} else {
						value.val[0] = 0;

						cvSet2D(hsv_mask, i, j, value);
					}

				}
			}

			cvSmooth( hsv_mask, hsv_mask, CV_MEDIAN, 25, 0, 0, 0 );

		}
		


		motion->Hotkey(key);

			if(motion->startClick){
				click->LeftClick(motion->startMove);
	//			click->RightClick();
			}	


			if (motion->startMove){
				motion->getMin(click->difference);
				motion->MoveTheMouse();
			} else {
				click->difference.x = 0;
				click->difference.y = 0;
			}

		
	
			//motion->DrawKereszt(frame);

			if (motion->left){
				cvFlip(hsv_mask,hsv_mask,1);
			}

			cvShowImage("Segment", hsv_mask);
		
	

			if (motion->firstMove){
				click->ConvexBurok(hsv_mask, frame);
				click->FindFingers(frame);
			}

		
			if (click->currentArea < 0.90*click->AVGarea && click->sampleCount > 149){
				motion->startMove = false;
//				cout << "motion stopped!!!!!!!" << endl;
			} else if (click->currentArea > 0.90*click->AVGarea && click->sampleCount > 149){
				motion->startMove = true;
//				cout << "motion STARTED!!!!!!!" << endl;
			}

//			cout << "AVG:" << click->AVGarea << endl;
//			cout << "cur:" << click->currentArea << endl;

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
