#include "Click.h"
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace std;

double Click::d(CvPoint point1, CvPoint point2){
	return sqrt(pow((double)(point2.x - point1.x), 2) + pow((double)(point2.y - point1.y), 2));
}

int Click::minArg(CvSeq* points){
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

void Click::ConvexBurok(IplImage* grayImg, IplImage *originalImg){
	try{
	// kont�r megkeres�se
		cvFindContours(grayImg, this->storange, &contours, 88, CV_RETR_CCOMP);

		// ha a kont�r �ltal k�rbez�rt ter�let nagyobb, mint 1000 (azaz biztos, hogy a kezet tal�ltuk meg),
		// csak akkor sz�molunk konvex burkot
		

		if(cvContourArea(contours) > 1000) {
			convexHull = cvConvexHull2(contours);
			hullCount = convexHull->total; // megadja, hogy h�ny sz�g� a konvex burok
		}
		
		
		// ha l�tezik a konvex burok (azaz nem csak a fekete h�tteret l�tjuk, hanem el�tte van a kez�nk), akkor...
		if(hullCount > 0) {
			// kovnvex burok pontjainak "ritk�t�sa" (hogy egy ujjat csak egy pont szimboliz�ljon)
			for(int i = 0; i < hullCount; i++ ) {
				point = **CV_GET_SEQ_ELEM(CvPoint*, convexHull, i);
				pointPrev = (i == 0) ? **CV_GET_SEQ_ELEM(CvPoint*, convexHull, hullCount-1) : **CV_GET_SEQ_ELEM(CvPoint*, convexHull, i-1);

				// csak azok a pontok maradnak meg, amelyek az al�bbi felt�telt kiel�g�tik
				if((d(pointPrev, point) >= dXY) && (abs(pointPrev.x - point.x) >= dX) && (abs(pointPrev.y - point.y) >= dY)) {
					cvSeqPush(points, &point); // a j� pontokat lerakjuk egy verembe (CvSeq haszn�lhat� veremk�nt is)
//					cvCircle(originalImg, point, 3, CV_RGB(0, 255, 0), 3); // a m�r kiritk�tott pontok megjelen�t�se
				}

				cvLine(originalImg, pointPrev, point, CV_RGB(0, 255, 0), 1); // konvex burok megjelen�t�se
			}
		}
	}catch (cv::Exception& e){
		cout << "cink van 1" << endl;
	}
		
}

void Click::FindFingers(IplImage *originalImg){
	try{
		if(points->total > 0) {
				// legkisebb y koordin�t�j� pont sorsz�ma
				min = minArg(points);

				// h�velykujjat (fingerTip1) �s mutat�ujjat (fingerTip2) szimboliz�l� pontok be�ll�t�sa
				fingerTip1 = *CV_GET_SEQ_ELEM(CvPoint, points, min+1);
				fingerTip2 = *CV_GET_SEQ_ELEM(CvPoint, points, min);

				// kurzor mozgat�sa (!!nincs felszorozva a manitor felbont�s�ra, csak kipr�b�ltam, hogy m�k�dik-e!!)
				//SetCursorPos(fingerTip2.x, fingerTip2.y);

				// kirajzol�s: h�velykujj k�kkel, mutat�ujj pirossal, a k�t pont t�vols�ga s�rg�val
				cvCircle(originalImg, fingerTip1, 3, CV_RGB(0, 0, 255), 3);
				cvCircle(originalImg, fingerTip2, 3, CV_RGB(255, 0, 0), 3);
				//cvLine(originalImg, fingerTip1, fingerTip2, CV_RGB(255, 255, 0), 1);

				// bal eg�rgomb akci�
			/*	if(d(fingerTip1, fingerTip2) > clickDistance) {
					cout << "Bal gomb lenyomva - " << d(fingerTip1, fingerTip2) << endl;
				} else {
					cout << "Bal gomb nincs lenyomva - " << d(fingerTip1, fingerTip2) << endl;
				}
			}
			*/
			// verem (pontsorozat) ki�r�t�se
			cvClearSeq(points);
		}
	}catch (cv::Exception& e){
		cout << "cink van 2" << endl;
	}
		
}

void Click::Hotkey(int key){
	switch (key){
	case 50:// 2 (inc clickDistance)
		clickDistance++; cout << "clickDistance: " << clickDistance << endl;
		break;
	case 49:// 1 (dec clickDistance)
		clickDistance--; cout << "clickDistance: " << clickDistance << endl;
		break;
	case 119:// w (inc dXY)
		dXY++; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;
		break;
	case 113:// q (dec dXY)
		dXY--; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;
		break;
	case 115:// s (inc dX)
		dX++; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;
		break;
	case 97:// a (dec dX)
		dX--; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;
		break;
	case 120:// x (inc dY)
		dY++; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;
		break;
	case 121:// y (dec dY) 
		dY--; cout << "dXY: " << dXY << ", dX: " << dX << ", dY: " << dY << endl;
		break;
	}
}