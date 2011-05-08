#include "Click.h"
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace std;

double Click::d(CvPoint point1, CvPoint point2) {
	return sqrt(pow((double)(point2.x - point1.x), 2) + pow((double)(point2.y - point1.y), 2));
}

int Click::minArg(CvSeq* points) {
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

void Click::ConvexBurok(IplImage* grayImg, IplImage *originalImg) {
	try {
		cvFindContours( grayImg, this->storange, &contours, sizeof(CvContour),
           CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );


		for( ; contours != 0; contours = contours->h_next ) {
			if(cvContourArea(contours) > 9000 && cvContourArea(contours) < 100000) {
				
				AVGareaFrames = 150;

				if (sampleCount % AVGareaFrames > 0){
					areaCounter += cvContourArea(contours);
					sampleCount++;
				} else {
					if(AVGarea == 0 || (AVGarea != 0 && (areaCounter / AVGareaFrames) > (0.75 * AVGarea) && (areaCounter / AVGareaFrames) < (1.25 * AVGarea))) AVGarea = areaCounter / AVGareaFrames;

					areaCounter = 0;
					sampleCount++;

					//cout << AVGarea << endl;
				}

				currentArea = cvContourArea(contours);

				convexHull = cvConvexHull2(contours);
				hullCount = convexHull->total; // megadja, hogy hány szögû a konvex burok
				break;
		   }
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
					cvCircle(originalImg, point, 3, CV_RGB(0, 255, 0), 3); // a már kiritkított pontok megjelenítése
				}

				cvLine(originalImg, pointPrev, point, CV_RGB(0, 255, 0), 1); // konvex burok megjelenítése
			}
		}
	} catch (cv::Exception& e) {
		cout << "cink van 1" << endl;
	}
		
}

void Click::FindFingers(IplImage *originalImg) {
	try{
		if(points->total > 0) {
			// legkisebb y koordinátájú pont sorszáma
			min = minArg(points);

			// hüvelykujjat (fingerTip1) és mutatóujjat (fingerTip2) szimbolizáló pontok beállítása
			fingerTip1 = *CV_GET_SEQ_ELEM(CvPoint, points, min+1);
			if(!jobbLe) fingerTip2 = *CV_GET_SEQ_ELEM(CvPoint, points, min);

			if (firstFrame) {
				PrewFingerTip2 = fingerTip2;
				firstFrame = false;
			}

			difference.x = fingerTip2.x - PrewFingerTip2.x;
			difference.y = fingerTip2.y - PrewFingerTip2.y;

			PrewFingerTip2 = fingerTip2;

			// kirajzolás: hüvelykujj kékkel, mutatóujj pirossal, a két pont távolsága sárgával
			cvCircle(originalImg, fingerTip1, 3, CV_RGB(0, 0, 255), 3);
			cvCircle(originalImg, fingerTip2, 3, CV_RGB(255, 0, 0), 3);
			cvLine(originalImg, fingerTip1, fingerTip2, CV_RGB(255, 255, 0), 1);

			// verem (pontsorozat) kiürítése
			cvClearSeq(points);
		}
	} catch (cv::Exception& e) {
		cout << "cink van 2" << endl;
	}
		
}

void Click::LeftClick(bool &startMove){
	//cout << d(fingerTip1, fingerTip2) << endl;

	if(d(fingerTip1, fingerTip2) >= clickDistance && !balLe && !jobbLe) {
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

		if(boolTime) {
			time(&distanceTime);
			boolTime=false;
			startMove=false;
		}

		balLe=true;
	} 
	
	if (difftime(time(&currentTime),distanceTime)>=2 && !boolTime){
		//cout<< "eltelt ido: "<< startMove<<endl;
			startMove=true;
			boolTime=true;
	}			

	if(d(fingerTip1, fingerTip2) < clickDistance && balLe){
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			balLe=false;
	}
}

void Click::RightClick() {
	if(currentArea > (1.1 * AVGarea) && !jobbLe && AVGarea != 0) {
		cout << "jobb le" << endl;
		
		PrewFingerTip2 = fingerTip2;
//		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
//		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		jobbLe=true;
	}

	if(currentArea <= (1.1 * AVGarea) && jobbLe) {
		cout << "jobb fel" << endl;

		fingerTip2 = PrewFingerTip2;
		jobbLe=false;
	}
}

void Click::Hotkey(int key) {
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
	case 110:// n 
		this->night = !this->night;
		if (!this->night)
			cout << "Using background substraction" <<endl;
		else
			cout << "2D normalized segment" <<endl;
		break;
		break;
	}
}