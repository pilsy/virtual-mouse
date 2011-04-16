#include "Motion.h"
#include <cv.h>
#include <iostream>
#include "Math.h"

void Motion::getMin(CvPoint fingerTip2){
	if(xMin != NULL) xMinPrev = xMin;
	if(yMin != NULL) yMinPrev = yMin;

	xMin = fingerTip2.x;
	if(xMinPrev != NULL && abs(xMin - xMinPrev) < 3) xMin = xMinPrev; // korrig�l�s, ha kell (hogy ne ugr�ljon x ir�nyban)

	yMin=fingerTip2.y;
	if(yMinPrev != NULL && abs(yMin - yMinPrev) < 3) yMin = yMinPrev; // korrig�l�s, ha kell (hogy ne ugr�ljon y ir�nyban)

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
Az eg�r mozgat�sa
*/
void Motion::MoveTheMouse(){

	// mozgat�s a sk�l�zott koordin�t�kra
	SetCursorPos((int)(((xMin - X1)*horizontal) / (X2 - X1)), (int)(((yMin - Y1)*vertical) / (Y2 - Y1)));

}

/*void Motion::DrawKereszt(IplImage  *frame){
		// kereszt kirajzol�sa
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

}*/

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