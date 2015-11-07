#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
		std::cout << "       " << argv[0] << std::endl;
		return -1;
	}
	
	// loading the image
	cv::Mat img = (argc == 2) ? cv::imread(argv[1]) : cv::imread("../rect.jpg");
	cv::imshow("img", img);

	
	// converting to hsv and splitting
	cv::Mat hsv;
	std::vector<cv::Mat> split;
	cv::cvtColor(img, hsv, CV_RGB2HSV);
	cv::split(hsv, split);
	cv::imshow("hue", split[0]);

	
	// threshing the image
	cv::Mat hueLower;
	cv::Mat hueUpper;
	cv::threshold(split[0], hueLower, 100, 255, CV_THRESH_BINARY);
	cv::threshold(split[0], hueUpper, 170, 255, CV_THRESH_BINARY_INV);
	cv::Mat thresh = hueUpper & hueLower;
	cv::imshow("thresh", thresh);

	
	// detect contours
	cv::Mat gray;
	std::vector<std::vector<cv::Point> > contours;
	cv::Canny(thresh, gray, 50, 150, 3); 
	findContours(thresh, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	cv::imshow("canny", gray);

	
	// approxPolyDP and filter the contours
	std::vector<std::vector<cv::Point> > rects;
	std::vector<cv::Point> approx;
	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours.at(i)), true) * 0.02, true);
		
		if(approx.size() == 4)
		{
			double maxCosine = 0;
			
			for(int j = 2; j<= 4; j++)
			{
				double cosine = fabs(cos(angle(approx[j%4], approx[j-2], approx[j-1])));
				maxCosine = MAX(maxCosine, cosine);
			}

			if(maxCosine < 0.3)
				rects.push_back(approx);
		}
	}
	
	//draw contours
	std::vector<cv::Vec4i> hierarchy;
	for(int i = 0; i < rects.size(); i++)
		cv::drawContours(img, rects, i, cv::Scalar(0, 255, 0), 2, 8, hierarchy, 0, cv::Point());

	// show the image
	cv::imshow("rectangles", img);
	cv::waitKey(0);

	return 0;
}

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    double bleh = atan(dy1/dx1)-atan(dy2/dx2);
    //std::cout << bleh << std::endl;
    return bleh;
}
