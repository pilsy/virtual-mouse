#include "Motion.h"
#include <cv.h>
#include <iostream>
#include "Math.h"

using namespace std;

void Motion::getMin(CvPoint diff){

	if (position[0] + diff.x > horizontal){
		position[0] = horizontal;
		diff.x = 0;
	}
	
	if (position[0] + diff.x < 0){
		position[0] = 0;
		diff.x = 0;
	}

	if (position[1] + diff.y > vertical){
		position[1] = vertical;
		diff.y = 0;
	}

	if (position[1] + diff.y < 0){
		position[1] = 0;
		diff.y = 0;
	}

	if (abs(diff.x) < 4 || abs(diff.x) > 70)
		diff.x = 0;

	if (abs(diff.y) < 4 || abs(diff.y) > 70)
		diff.y = 0;
	
	if (left){
		position[0] += 640 - speed*diff.x;
		position[1] += speed*diff.y;
	} else {
		position[0] += speed*diff.x;
		position[1] += speed*diff.y;
	}


	PrewPosition[0] = position[0];
	PrewPosition[1] = position[1];

}

void Motion::GetDesktopResolution(int& horizontal, int& vertical){
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);

   horizontal = desktop.right;
   vertical = desktop.bottom;
}

/*
Az egér mozgatása
*/
void Motion::MoveTheMouse(){

	// mozgatás a skálázott koordinátákra
	//SetCursorPos((int)(((xMin - X1)*horizontal) / (X2 - X1)), (int)(((yMin - Y1)*vertical) / (Y2 - Y1)));
	SetCursorPos(position[0], position[1]);

}


void Motion::Hotkey(int key){
	switch (key){
	case 'd':
		stop = 0;
		break;
	case 'p':
		startMove = !startMove;
		startClick=!startClick;
		firstMove = !firstMove;
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