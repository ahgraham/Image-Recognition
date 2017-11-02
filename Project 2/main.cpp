#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

// Mouse position callback function
void on_mouse(int evt, int x, int y, int flags, void* param) // Callback frunction for mouse click
{
    if(evt==CV_EVENT_LBUTTONDOWN)
        printf("Clicked at (%d,%d)\n", x, y);
}

// get distance between points as a scalar
double distBetweenPoints(Point_<float> p1, Point_<float> p2 ){
    Point diff = p1 - p2;
    return sqrt(diff.x*diff.x+diff.y*diff.y);
}

// Take an image and return a binary image
IplImage* GetThresholdedImage(IplImage* img) {

    // Thresholded image
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

    // Do the thresholding
    cvInRangeS(img, cvScalar(0, 0, 0), cvScalar(10, 10, 10), imgThreshed);

    // Return thresholded image
    return imgThreshed;
}

int main() {
    // Load image
    // *KEEP IN MIND* IplImage is legacy, as well as functions that operate with it. Clean up code later to use Mat.
    IplImage* img = cvLoadImage("/Users/agraham/Documents/Development/Creating AR/Images/Image 2.jpg");


    // Create window for image, and display image on that window
    cvNamedWindow("Original");
    cvShowImage("Original", img);

    // Create window for thresholded image, and display it
    IplImage *imgThresh = GetThresholdedImage(img); // Create thresholded image
     // Create matrix from thresholded image
    cvNamedWindow("Thresholded");

    // Create mouse callback for debugging
    cvSetMouseCallback("Thresholded", on_mouse, 0); //Add callback for mouse click

    cvShowImage("Thresholded", imgThresh);

    // Create Mat, for further operations
    Mat imgThreshMat = cvarrToMat(imgThresh);

    /// Convert image to gray and blur it
    Mat grayImg, canny_output;
    grayImg = imgThreshMat; // Image is already greyscale, no need for conversion
    blur( grayImg, grayImg, Size(3,3) );
    // Detect edges with canny *not necessary here
    //Canny( grayImg, canny_output, 100, 200, 3 );
    canny_output = grayImg;

    // Find contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    /// Get the moments
    // * FIX * Do not get moments, get the edges from the contour
    // In this context, contours.size() = 2 (there are two objects)
    vector<Moments> mu(contours.size() );
    vector<vector<Point> > comparisons = contours;
    for( int i = 0; i < contours.size(); i++ )
    {
        // This line required
        mu[i] = moments( contours[i], false );
        // Get the max and min of contours[i], for later comparison
        for( int x = 0; x < contours[i].size(); x++ ) {
            // Move any min to the first element, and max to the second element
            if (contours[i][x].x < comparisons[i][0].x) {
                comparisons[i][0].x = contours[i][x].x;
            }
            if (contours[i][x].y < comparisons[i][0].y) {
                comparisons[i][0].y = contours[i][x].y;
            }
            // Move any max to the first element
            if (contours[i][x].x > comparisons[i][0].x) {
                comparisons[i][1].x = contours[i][x].x;
            }
            if (contours[i][x].y > comparisons[i][0].y) {
                comparisons[i][1].y = contours[i][x].y;
            }
            // Remove elements not equal to 0 or 1
            comparisons[i].erase(comparisons[i].begin() + 2, comparisons[i].end());
        }
    }

    ///  Get the mass centers:
    vector<Point2f> mc( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    {
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
        // For debugging
        //cout << mc;
    }

    /// Compare the mass centers to the edges, to find the corner
    vector<Point> enclosed_pair(mc.size()); // Contains the pairs (add when more than 1 pair)
    for( int i = 0; i < contours.size(); i++ ){
        int left = 0;
        int right = 0;
        // Compare mass centers to max and min, to see which is closer
        if(distBetweenPoints(mc[i],comparisons[i][0]) < distBetweenPoints(mc[i],comparisons[i][1])){
            left = 1;
            // Remove right element (max), leaving only the corner
            comparisons[i].erase(comparisons[i].begin()+1); // *THIS CAN BE REMOVED* with the addition of enclosed_pair
            enclosed_pair[0] = comparisons[i][0];
        }
        else {
            right = 1;
            // Remove left element (min), leaving only the corner
            comparisons[i].erase(comparisons[i].begin());  // *THIS CAN BE REMOVED* with the addition of enclosed_pair
            enclosed_pair[1] = comparisons[i][1];

        }
        // For debugging
        cout << contours[i] << "\n";
        cout << comparisons[i] << "\n";
        printf("(%d,%d) as (left,right)\n", left, right);
        cout << enclosed_pair;
    }



    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    RNG rng(12345);
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
        circle( drawing, mc[i], 4, color, -1, 8, 0 );
        circle( drawing, comparisons[i][0], 4, color, -1, 8, 0 );
    }

    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );

    /// Get the inner image
    int x = enclosed_pair[0].x,
            y = enclosed_pair[0].y,
            width = enclosed_pair[1].x - x,
            height = enclosed_pair[1].y - y;

    Mat imgMat, img2;
    imgMat = cvarrToMat(img); // Create Mat of original image;
    cout << x << endl << y << endl << width << endl << height;
    // Include 5 to each parameter to not include the brackets
    img2 = imgMat(Rect(x+5, y+5, width-10, height-10));
    // Show in a window
    namedWindow( "Inner Image", CV_WINDOW_AUTOSIZE );
    imshow( "Inner Image", img2 );


    // Wait forever
    cvWaitKey(0);
}