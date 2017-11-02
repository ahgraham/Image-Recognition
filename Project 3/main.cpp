#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

int main() {
    // Initialize variables
    Mat origImg, hsvImg;

    // Load the image
    origImg = imread("/Users/agraham/Documents/Development/Creating AR/Project 3/man-in-a-room.jpg");

    // Convert to HSV
    cvtColor(origImg, hsvImg, CV_BGR2GRAY);
    //imshow("hsv img", hsvImg);

    // Threshold for certain skin range
    CvScalar  hsv_min = cvScalar(0, 10, 60, 0);
    CvScalar  hsv_max = cvScalar(20, 0.150, 255, 0);
    // Convert image Mat to iplImage for cvInRangeS function
    IplImage* hsvImg2 = new IplImage(hsvImg);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(hsvImg2), 8, 1); // This will be the output of inRangeFunction
    cvInRangeS(hsvImg2, hsv_min, hsv_max, imgThreshed);
    // Display the thresholded image
    cvNamedWindow("Thresholded");
    cvShowImage("Thresholded", imgThreshed);




    // Get the second and third largest blobs

    // Create boxes around these blobs of size average radius from center of blob

    // Display the image with the hands in the boxes

    // Wait forever
    cvWaitKey(0);
}