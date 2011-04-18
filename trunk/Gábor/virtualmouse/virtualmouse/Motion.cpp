#include "Motion.h"
#include <cv.h>
#include <iostream>
#include "Math.h"

using namespace std;

void Motion::getMin(CvPoint fingerTip2){
	if(xMin != NULL) xMinPrev = xMin;
	if(yMin != NULL) yMinPrev = yMin;

	if (this->left){
		xMin = 640 - fingerTip2.x;
		if(xMinPrev != NULL && abs(xMin - xMinPrev) < 6) xMin = xMinPrev; // korrigálás, ha kell (hogy ne ugráljon x irányban)
	} else {
		xMin = fingerTip2.x;
		if(xMinPrev != NULL && abs(xMin - xMinPrev) < 6) xMin = xMinPrev; // korrigálás, ha kell (hogy ne ugráljon x irányban)
	}

	yMin=fingerTip2.y;
	if(yMinPrev != NULL && abs(yMin - yMinPrev) < 6) yMin = yMinPrev; // korrigálás, ha kell (hogy ne ugráljon y irányban)

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

void Motion::DrawKereszt(IplImage  *frame){
		// kereszt kirajzolása
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

void Motion::Hotkey(int key){
	switch (key){
	case 'd':
		stop = 0;
		break;
	case 'p':
		startMove = !startMove;
		startClick=!startClick;
		break;
	case 108:// l 
		this->left = !this->left;

		if (this->left){
			cout << "Left handed mouse Enabled!" << endl;
		} else {
			cout << "Left handed mouse Disabled!" << endl;
		}
		break;
		}
}