#ifndef Motion_h
#define Motion_h


#include <cxcore.h>
#include <highgui.h>

class Motion {

	private:
		int xMin, yMin;				// Minimumhelyek koordinátái
		int width, height;
		int X1, X2, Y1, Y2, R ;
		int horizontal, vertical;
		CvSeq* circles;	
		CvScalar value;
	public:
		int stop;		
		bool startMove;

		Motion(int w,int h){
			X1=0, X2=0, Y1=0, Y2=0, R=0 ;
			horizontal = 0, vertical = 0;
			xMin = 200, yMin = 200;
			width = w, height = h;
			stop = -1;
			startMove = false;
		}
		~Motion(){
		}

		void getMin(IplImage* segment);

		void GetDesktopResolution(int& horizontal, int& vertical);

		void MoveTheMouse();

		void GetCircleInfo();

		void DrawKereszt(IplImage  *frame);

		void FindCircle(IplImage* segment,CvMemStorage* storage);

		void CutRegion(IplImage *segment);

		void Hotkey(int key);

};

#endif Motion_h
