// bgremove.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "bgremove.h"
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <vector>



#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include<opencv2/core.hpp>



//C
#include <stdio.h>
//C++
//#include <iostream>
#include <sstream>

#include "base64.h"


using namespace std;
using namespace cv; 

Mat bgRemove(Mat imgSource, int top, int left, int width, int height, int iters);

Mat bgRemove(Mat imgSource, int top, int left, int width, int height, int iters)
{
		

	int r = imgSource.rows;
	int c = imgSource.cols;

	Point p1 = Point(c / 100, r / 100);
	Point p2 = Point(c - c / 100, r - r / 100);
	Rect rect = Rect(p1, p2);

	Mat mask = Mat();
	Mat fgdModel = Mat();
	Mat bgdModel = Mat();

	Mat imgC3 = imgSource;// Mat();

	//mask.create(imgSource.size(), CV_8UC1);

	//cvtColor(imgSource, imgC3, COLOR_RGBA2RGB);

	grabCut(imgC3, mask, rect, bgdModel, fgdModel, iters, GC_INIT_WITH_RECT);

		
	//CONVERT TO PNG 
	Mat source = Mat(1, 1, CV_8U, Scalar(3.0));
	compare(mask, source/* GC_PR_FGD */, mask, CMP_EQ);	
	Mat foreground = Mat(imgSource.size(), CV_8UC3, Scalar(255,	255, 255, 255));

	
	imgSource.copyTo(foreground, mask);

	return foreground;
}



extern "C" BGREMOVE_API char*  __stdcall  fnbgRemove(char* base64img,int top, int left, int width, int height, int iterations)
{
	string encoded_string = base64img; 

	string decoded_string = base64_decode(encoded_string);
	vector<uchar> data(decoded_string.begin(), decoded_string.end());
	Mat img = imdecode(data, IMREAD_UNCHANGED);

	Mat dest;

	//imshow("Image", img);
	

	try {
		dest = bgRemove(img, top, left, width,height, iterations);
		
		
	}
	catch (Exception ex)
	{
		return NULL;
	}
	
	

	int params[3] = { 0 };
	params[0] = IMWRITE_PNG_COMPRESSION; // IMWRITE_PNG_BILEVEL;//IMWRITE_JPEG_QUALITY;
	params[1] = 0;

	
	vector<uchar> buf;
	bool code = cv::imencode(".png", dest, buf, std::vector<int>(params, params + 2));
	uchar* result = reinterpret_cast<uchar*> (&buf[0]);

	string strretbase64 =base64_encode(result, buf.size());

	//waitKey();

	char* pcharRet = new char[strretbase64.length()];
	strcpy(pcharRet, strretbase64.c_str());
	return pcharRet;

	
}







//
//
//static const std::string base64_chars =
//"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//"abcdefghijklmnopqrstuvwxyz"
//"0123456789+/";
//
//
//static inline bool is_base64(unsigned char c) {
//	return (isalnum(c) || (c == '+') || (c == '/'));
//}
//
//std::string base64_decode(std::string const& encoded_string) {
//	int in_len = encoded_string.size();
//	int i = 0;
//	int j = 0;
//	int in_ = 0;
//	unsigned char char_array_4[4], char_array_3[3];
//	std::string ret;
//
//	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
//		char_array_4[i++] = encoded_string[in_]; in_++;
//		if (i == 4) {
//			for (i = 0; i < 4; i++)
//				char_array_4[i] = base64_chars.find(char_array_4[i]);
//
//			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
//			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
//			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
//
//			for (i = 0; (i < 3); i++)
//				ret += char_array_3[i];
//			i = 0;
//		}
//	}
//
//	if (i) {
//		for (j = i; j < 4; j++)
//			char_array_4[j] = 0;
//
//		for (j = 0; j < 4; j++)
//			char_array_4[j] = base64_chars.find(char_array_4[j]);
//
//		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
//		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
//		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
//
//		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
//	}
//
//	return ret;
//}
