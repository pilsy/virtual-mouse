#ifndef Click_h
#define Click_h


#include <cxcore.h>
#include <highgui.h>
#include <time.h>

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
		time_t distanceTime, currentTime;
		int clickDistance;
		CvMemStorage* storange;
		bool boolTime;
		bool balLe;
		bool jobbLe;

	public:
		CvPoint fingerTip1;
		CvPoint fingerTip2;
		CvPoint PrewFingerTip2;
		CvPoint difference;
		int AVGarea, sampleCount, areaCounter, currentArea;
		bool segment, night, firstFrame;

		Click(CvMemStorage* storange){
			this->storange=storange;
			contours=NULL;
			convexHull=NULL;
			hullCount=0;
			points = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), this->storange);
			dXY = 30, dX = 5, dY = 5;
			min=0;
			clickDistance = 175;
			segment = true;
			night = false;
			firstFrame = true;
			boolTime=true;
			balLe=false;
			jobbLe=false;
			sampleCount = 1;
			AVGarea = 0;
			areaCounter = 0;
			currentArea = 0;
		}
		~Click(){
		
		}

		double d(CvPoint point1, CvPoint point2);

		int minArg(CvSeq* points);

		void ConvexBurok(IplImage* grayImg, IplImage *originalImg);

		void FindFingers(IplImage  *originalImg);

		void Clicking(bool &);

		void RightClick();

		void Hotkey(int key);
};

#endif Click_h
