#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace std;

int main( int argc, char **argv )
{
	CvCapture *capture = 0;
	IplImage  *frame = 0;
	int       key = 0;

	/* initialize camera */
	capture = cvCaptureFromCAM( 0 );

	/* always check */
	if ( !capture ) {
		cout << "Nem sikerult megnyitni a webkamerat!" << endl;
		return 1;
	} else {
		cout << "Kilepeshez nyomd meg a 'q' gombot!" << endl;
	}

	/* create a window for the video */
	cvNamedWindow( "Webkamera", CV_WINDOW_AUTOSIZE );

	while( key != 'q' ) {
	/* get a frame */
		frame = cvQueryFrame( capture );

		/* always check */
		if( !frame ) break;

		/* display current frame */
		cvShowImage( "Webkamera", frame );

		/* exit if user press 'q' */
		key = cvWaitKey( 1 );
	}

	/* free memory */
	cvDestroyWindow( "Webkamera" );
	cvReleaseCapture( &capture );

	return 0;
}