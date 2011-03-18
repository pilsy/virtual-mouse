// VirtualMouse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <math.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace std;

// két pont távolságát számító eljárás
double d(CvPoint point1, CvPoint point2) {
	return sqrt(pow((double)(point2.x - point1.x), 2) + pow((double)(point2.y - point1.y), 2));
}

// pontok sorozatából a minimális y koordinátájú pont _indexének_ visszaadása
int minArg(CvSeq* points) {
	int min_i = 0;
	int min_y = (*CV_GET_SEQ_ELEM(CvPoint, points, min_i)).y;
				
	for(int i = 1; i < points->total; i++) {
		if((*CV_GET_SEQ_ELEM(CvPoint, points, i)).y < min_y) {
			min_i = i;
			min_y = (*CV_GET_SEQ_ELEM(CvPoint, points, i)).y;
		}
	}

	return min_i;
}

int _tmain(int argc, _TCHAR* argv[])
{
	CvCapture* capture = cvCreateCameraCapture(0);
	CvMemStorage* storage = cvCreateMemStorage();
	CvPoint fingerTip1;
	CvPoint fingerTip2;
	CvPoint point;
	CvPoint pointPrev;
	CvSeq* points = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);
	CvSeq* contours;
	CvSeq* convexHull;
	IplImage* originalImg;
	IplImage* grayImg;
	char k;
	int hullCount = 0;
	int min;
	int clickDistance = 120;
	int dXY = 25, dX = 5, dY = 5;

	cout << "Kilepes: ESC" << endl;
	cout << "------------" << endl;
	cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;

	cvNamedWindow("Virtual Mouse", CV_WINDOW_AUTOSIZE);

	while(1) {
		// kamera képének lementése
		originalImg = cvQueryFrame(capture);
		if(!originalImg) break;

		// átalakítás szürkeárnyalatosra
		grayImg = cvCreateImage(cvSize(originalImg->width, originalImg->height), IPL_DEPTH_8U, 1);
		cvCvtColor(originalImg, grayImg, CV_RGB2GRAY);

		// küszöbölés
		cvThreshold(grayImg, grayImg, 0, 255, CV_THRESH_OTSU);

		// kontúr megkeresése
		cvFindContours(grayImg, storage, &contours, 88, CV_RETR_CCOMP);

		// ha a kontúr által körbezárt terület nagyobb, mint 1000 (azaz biztos, hogy a kezet találtuk meg),
		// csak akkor számolunk konvex burkot
		if(cvContourArea(contours) > 1000) {
			convexHull = cvConvexHull2(contours);
			hullCount = convexHull->total; // megadja, hogy hány szögû a konvex burok
		}

		// ha létezik a konvex burok (azaz nem csak a fekete hátteret látjuk, hanem elõtte van a kezünk), akkor...
		if(hullCount > 0) {
			// kovnvex burok pontjainak "ritkítása" (hogy egy ujjat csak egy pont szimbolizáljon)
			for(int i = 0; i < hullCount; i++ ) {
				point = **CV_GET_SEQ_ELEM(CvPoint*, convexHull, i);
				pointPrev = (i == 0) ? **CV_GET_SEQ_ELEM(CvPoint*, convexHull, hullCount-1) : **CV_GET_SEQ_ELEM(CvPoint*, convexHull, i-1);

				// csak azok a pontok maradnak meg, amelyek az alábbi feltételt kielégítik
				if((d(pointPrev, point) >= dXY) && (abs(pointPrev.x - point.x) >= dX) && (abs(pointPrev.y - point.y) >= dY)) {
					cvSeqPush(points, &point); // a jó pontokat lerakjuk egy verembe (CvSeq használható veremként is)
//					cvCircle(originalImg, point, 3, CV_RGB(0, 255, 0), 3); // a már kiritkított pontok megjelenítése
				}

//				cvLine(originalImg, pointPrev, point, CV_RGB(0, 255, 0), 1); // konvex burok megjelenítése
			}

			if(points->total > 0) {
				// legkisebb y koordinátájú pont sorszáma
				min = minArg(points);

				// hüvelykujjat (fingerTip1) és mutatóujjat (fingerTip2) szimbolizáló pontok beállítása
				fingerTip1 = *CV_GET_SEQ_ELEM(CvPoint, points, min+1);
				fingerTip2 = *CV_GET_SEQ_ELEM(CvPoint, points, min);

				// kurzor mozgatása (!!nincs felszorozva a manitor felbontására, csak kipróbáltam, hogy mûködik-e!!)
				SetCursorPos(fingerTip2.x, fingerTip2.y);

				// kirajzolás: hüvelykujj kékkel, mutatóujj pirossal, a két pont távolsága sárgával
				cvCircle(originalImg, fingerTip1, 3, CV_RGB(0, 0, 255), 3);
				cvCircle(originalImg, fingerTip2, 3, CV_RGB(255, 0, 0), 3);
				cvLine(originalImg, fingerTip1, fingerTip2, CV_RGB(255, 255, 0), 1);

				// bal egérgomb akció
				if(d(fingerTip1, fingerTip2) > clickDistance) {
					cout << "Bal gomb lenyomva - " << d(fingerTip1, fingerTip2) << endl;
				} else {
					cout << "Bal gomb nincs lenyomva - " << d(fingerTip1, fingerTip2) << endl;
				}
			}

			// verem (pontsorozat) kiürítése
			cvClearSeq(points);
		}

		cvShowImage("Virtual Mouse", originalImg);

		k = cvWaitKey(10);
		// esc
		if(k == 27) break;
		// 50 (inc clickDistance)
		else if(k == 50) {clickDistance++; cout << "clickDistance: " << clickDistance << endl;}
		// 49 (dec clickDistance)
		else if(k == 49) {clickDistance--; cout << "clickDistance: " << clickDistance << endl;}
		// w (inc dXY)
		else if(k == 119) {dXY++; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;}
		// q (dec dXY)
		else if(k == 113) {dXY--; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;}
		// s (inc dX)
		else if(k == 115) {dX++; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;}
		// a (dec dX)
		else if(k == 97) {dX--; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;}
		// x (inc dY)
		else if(k == 120) {dY++; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;}
		// y (dec dY)
		else if(k == 121) {dY--; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;}
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("Virtual Mouse");

	return 0;
}