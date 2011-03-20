#ifndef Click_h
#define Click_h


#include <cxcore.h>
#include <highgui.h>

class Click {

	private:
		CvSeq* contours;
		CvSeq* convexHull;
		int hullCount;
		CvPoint point;
		CvPoint pointPrev;
		CvSeq* points;
		int dXY, dX, dY;
		int min;
		CvPoint fingerTip1;
		CvPoint fingerTip2;
		int clickDistance;
		CvMemStorage* storange;
	public:
		
		Click(CvMemStorage* storange){
			this->storange=storange;
			contours=NULL;
			convexHull=NULL;
			hullCount=0;
			points = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), this->storange);
			dXY = 30, dX = 5, dY = 5;
			min=0;
			clickDistance = 175;
		}
		~Click(){
		
		}

		double d(CvPoint point1, CvPoint point2);

		int minArg(CvSeq* points);

		void ConvexBurok(IplImage* grayImg);

		void FindFingers(IplImage  *originalImg);

		void Hotkey(int key);
};

#endif Click_h
