#ifndef Motion_h
#define Motion_h


#include <cxcore.h>
#include <highgui.h>

class Motion {

	private:
		int xMinPrev, yMinPrev;
		int xMin, yMin;				// Minimumhelyek koordinátái
		int width, height;
		int X1, X2, Y1, Y2, R ;
		CvSeq* circles;	
		CvScalar value;
	public:
		int horizontal, vertical;
		int stop;		
		bool startMove;

		Motion(int w,int h){
			X1=0, X2=w, Y1=0, Y2=h, R=0 ;
			horizontal = 20, vertical = 20;
			xMin = 200, yMin = 200;
			width = w-10, height = h-10;
			stop = -1;
			startMove = false;
		}
		~Motion(){
		}

		void getMin(CvPoint fingerTip2);

		void GetDesktopResolution(int& horizontal, int& vertical);

		void MoveTheMouse();

		void DrawKereszt(IplImage  *frame);

		void Hotkey(int key);

};

#endif Motion_h
