/*************************************************************************
    > File Name: region_test.cpp
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn 
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2017-06-23 11:31:07
 ************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
int main(int argc, char* argv[])
{
    string ImgNamesLists[100];
    char str[200];
    FILE *fImgList = fopen(argv[1], "rb");
    if (NULL == fImgList)
        return -1;
    int count = 0;
    while (fgets(str, 200, fImgList) != NULL && count < 100) {
        memset(str + strlen(str)-1, 0, 1);
        ImgNamesLists[count] = str;
        count ++;
    }

    Mat srcImage;
    int sx, sy, ex, ey;//start, end
    for(int i = 0; i < count; i ++)
    {
        srcImage = imread(ImgNamesLists[i]);
        int height = srcImage.rows;
        int width = srcImage.cols;
        //sy = height / 4 + 20;
        //sx = width / 3 + 50;
        //ey = sy + 300;
        //ex = sx + 200;
        sy = 205;
        sx = 390;
        ey = 460;
        ex = 570;
        rectangle(srcImage, cvPoint(sx, sy), cvPoint(ex, ey), cvScalar(0, 255, 0));
        imshow("src", srcImage);
        cout<<ImgNamesLists[i]<<endl;
        waitKey(500);
    } 
    cout<<"height->[ "<<sy<<", "<<ey<<" ]"<<endl;
    cout<<"width-->[ "<<sx<<", "<<ex<<" ]"<<endl;
    
    /*
    srcImage = imread(ImgNamesLists[2]);
    int sy = srcImage.rows / 4 + 20;
    int sx = srcImage.cols / 3 + 50;
    //int ey = sy + 300;
    //int ex = sx + 200;
	Mat roiImage;
	Rect rect(sx, sy, 200, 300);
	srcImage(rect).copyTo(roiImage);
	imshow("roi", roiImage);
	waitKey(0);
	imwrite("roi.jpg", roiImage);
    
    // Save multi ROI area
    
	std::vector<cv::Rect> rects;
	for (size_t i = 0; i < 4; i++)
	{
		rects.push_back(cv::Rect(i*10, i*10, 50, 50));  
	}


	std::vector<cv::Mat> subImages;
	for(int i = 0; i < rects.size(); i++)
	{
		cv::Mat tempImg;
		srcImage(rects[i]).copyTo(tempImg);
		subImages.push_back(tempImg);

		//cv::imwrite("roi.jpg", subImages[i]);

		cv::imshow("subImage", subImages[i]);
		cv::waitKey(0);
	}
    */
    
	return 0;
}
