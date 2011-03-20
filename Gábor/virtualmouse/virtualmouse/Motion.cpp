#include "Motion.h"
#include <cv.h>
#include <iostream>

void Motion::getMin(IplImage* segment){
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

void Motion::GetDesktopResolution(int& horizontal, int& vertical){
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
Az egér mozgatása
*/
void Motion::MoveTheMouse(){

	// mozgatás a skálázott koordinátákra
	SetCursorPos((int)(((xMin - X1)*horizontal) / (X2 - X1)), (int)(((yMin - Y1)*vertical) / (Y2 - Y1)));

}

void Motion::GetCircleInfo(){
	std::cout << "Circles found!" << std::endl;

	stop = 1;
	float *p1 = (float *)cvGetSeqElem( circles, 0);
	float *p2 = (float *)cvGetSeqElem( circles, 1);

	/*
	körök rendezése a skálázáshoz
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
	legnagyobb sugár lementése
	*/
	if (p1[2] > p2[2]){
		R = (int)p1[2];
	}else{
		R = (int)p2[2];
	}
}

void Motion::DrawKereszt(IplImage  *frame){
	/*
		kereszt kirajzolása
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

}

void Motion::FindCircle(IplImage* segment,CvMemStorage* storage){
	circles = cvHoughCircles( segment, storage, CV_HOUGH_GRADIENT, 1, 400, 10, 10, 17, 20);

			for( int i = 0; i < circles->total; i++ )
			{
				 float *p = (float *)cvGetSeqElem( circles, i);
			}


			if (circles->total == 2){

				GetCircleInfo();

			}
}

void Motion::CutRegion(IplImage *segment){
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

void Motion::Hotkey(int key){
	switch (key){
	case 'd':
		stop = 0;
		break;
	case 'p':
		startMove = !startMove;
		break;
		}
}