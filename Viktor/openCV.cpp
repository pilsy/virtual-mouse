#include <cv.h>
#include <ml.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

static int width = 640, height = 480;
int xMin = 200, yMin = 200;				// Minimumhelyek koordin�t�i
int horizontal = 0, vertical = 0;		// k�perny� felbont�s
int X1, X2, Y1, Y2, R, stop = -1;		// k�r param�terek �s logikai �rt�k a keres�shez
CvSeq* circles;							// k�r�k


/*
Minim�lis koordin�t�t megkeres� elj�r�s
*/
void getMin(IplImage* segment){
	CvScalar value;

	for ( int i = 0; i < height ; i++ ){

		for ( int j = 0; j < width; j++ ){

			value = cvGet2D(segment,i,j);

			if (value.val[0] == 255){
				xMin = j;
				yMin = i;
				return;
			}

		}

	}
}

/*
felbont�s lek�r�se
*/
void GetDesktopResolution(int& horizontal, int& vertical){
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}

/*
Az eg�r mozgat�sa
*/
void MoveTheMouse(){

	// mozgat�s a sk�l�zott koordin�t�kra
	SetCursorPos((int)(((xMin - X1)*horizontal) / (X2 - X1)), (int)(((yMin - Y1)*vertical) / (Y2 - Y1)));

}

/*
korparam�terek kinyer�se
*/
void GetCircleInfo(){

	cout << "Circles found!" << endl;

	stop = 1;
	float *p1 = (float *)cvGetSeqElem( circles, 0);
	float *p2 = (float *)cvGetSeqElem( circles, 1);

	/*
	k�r�k rendez�se a sk�l�z�shoz
	*/
	if (p1[0] > p2[0]){
		X1 = (int)p2[0];
		X2 = (int)p1[0];
	} else {
		X1 = (int)p1[0];
		X2 = (int)p2[0];
	}

	if (p1[1] > p2[1]){
		Y1 = (int)p2[1];
		Y2 = (int)p1[1];
	} else {
		Y1 = (int)p1[1];
		Y2 = (int)p2[1];
	}

	/*
	legnagyobb sug�r lement�se
	*/
	if (p1[2] > p2[2]){
		R = (int)p1[2];
	}else{
		R = (int)p2[2];
	}

}



 
int main( int argc, char **argv )
{
    CvCapture *capture = 0;
    IplImage  *frame = NULL;
	IplImage  *grey  = NULL;
    IplImage  *edges = NULL;
	IplImage  *segment = NULL;
	CvSize size = cvSize( width, height);
	CvSize kernelGauss = cvSize( 5, 5);
	CvMemStorage* storage = cvCreateMemStorage(0);
    int key = 0;
	CvScalar value;
	bool startMove = false;

	GetDesktopResolution(horizontal, vertical);
 
    /* initialize camera */
    capture = cvCaptureFromCAM( 1 );

	cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,30);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,width);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,height);
 
    /* siker�lt-e megnyitni a camot */
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return 1;
    }
 
    cvNamedWindow( "Original", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Threshold", CV_WINDOW_AUTOSIZE );

	    grey  = cvCreateImage(size, IPL_DEPTH_8U, 1);
		edges = cvCreateImage(size, IPL_DEPTH_8U, 1);
		segment = cvCreateImage(size, IPL_DEPTH_8U, 1);
 
    while( key != 'q' ) {
       
		// aktu�lis frame
        frame = cvQueryFrame( capture );
 
        if( !frame ) break;


		cvCvtColor(frame, grey, CV_BGR2GRAY);
		cvSmooth(grey, grey, CV_GAUSSIAN, 11, 11, 2, 0);
		cvThreshold(grey, segment, 140, 255, 0);

		if (key == 'd'){
			stop = 0;
		}

		if (stop == 0){

			circles = cvHoughCircles( segment, storage, CV_HOUGH_GRADIENT, 1, 400, 10, 10, 17, 20);

			for( int i = 0; i < circles->total; i++ )
			{
				 float *p = (float *)cvGetSeqElem( circles, i);
			}


			if (circles->total == 2){

				GetCircleInfo();

			}
		}

		if (stop == 1){

			for ( int i = 0; i < height; i++ ){

				for ( int j = 0; j < width; j++ ){

					if (j < X1 || j > X2 || Y1 > i || Y2 < i ){

						value.val[0] = 0;
						cvSet2D(segment, i,j,value);
					} 

					int a1 = (X1-j)*(X1-j);
					int b1 = (Y1-i)*(Y1-i);
					int r = R*R*2;

					int a2 = (X2-j)*(X2-j);
					int b2 = (Y2-i)*(Y2-i);

					if ( (a1 + b1 - r) <= 0 || (a2 + b2 - r) <= 0){

						value.val[0] = 0;
						cvSet2D(segment, i,j,value);
					} 

				}

			}

		}

		cvCanny( segment, edges, 60, 150, 3);

		getMin(segment);

		if (key == 'p'){
			startMove = !startMove;
		}

		if (startMove){

			MoveTheMouse();
		}

		/*
		kereszt kirajzol�sa
		*/
		if (xMin > 5 && yMin > 5 && (xMin < width-5 && yMin < height -5 )){
		value.val[0] = 0;
		value.val[1] = 0;
		value.val[2] = 255;
		cvSet2D(frame, yMin,xMin,value);
		cvSet2D(frame, yMin+1,xMin,value);
		cvSet2D(frame, yMin-1,xMin,value);
		cvSet2D(frame, yMin,xMin+1,value);
		cvSet2D(frame, yMin,xMin-1,value);
		cvSet2D(frame, yMin+2,xMin,value);
		cvSet2D(frame, yMin-2,xMin,value);
		cvSet2D(frame, yMin,xMin+2,value);
		cvSet2D(frame, yMin,xMin-2,value);
		cvSet2D(frame, yMin+3,xMin,value);
		cvSet2D(frame, yMin-3,xMin,value);
		cvSet2D(frame, yMin,xMin+3,value);
		cvSet2D(frame, yMin,xMin-3,value);
		cvSet2D(frame, yMin+4,xMin,value);
		cvSet2D(frame, yMin-4,xMin,value);
		cvSet2D(frame, yMin,xMin+4,value);
		cvSet2D(frame, yMin,xMin-4,value);
		}
		
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
 
    return 0;
}