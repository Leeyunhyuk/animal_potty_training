#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "SerialTranse.h"
#define MAX_DATA_LENGTH 10

SerialTranse arduino("/dev/ttyACM0");
SerialTranse node("/dev/ttyCPP");

using namespace cv;
using namespace std;

// setting for picamera pipeline
std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
           std::to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(framerate) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
           std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

// setting for picamera resolution
int capture_width = 1280 ;
int capture_height = 720 ;
int display_width = 640 ;
int display_height = 360 ;
int framerate = 30 ;
int flip_method = 2 ;

// setting for picamera pipeline
std::string pipeline = gstreamer_pipeline(capture_width,
capture_height,
display_width,
display_height,
framerate,
flip_method);

int check_dung = 0;
int threshold1 = 26;
int threshold2 = 30;
char output[MAX_DATA_LENGTH];

Vec3b lower_blue1, upper_blue1, lower_blue2, upper_blue2, lower_blue3, upper_blue3;
Vec3b lower, upper;

Mat frame;
Mat img_color;
Mat pt, pt2, dummy2;
Mat image;

vector<Point> pt_vt;

const char *stop = "p";
const char *resume = "a";
const char *go = "b";

int before_area = 0;
int white_pixels = 0;
int feed = 0;
int setting_region;
void onMouse(int event, int x, int y, int flags, void* p);
void dummy(int, void*);
void set_color();
void set_color2();
int setting_range(Mat& origen, Mat& target);

void find_white_pixel(Mat gray);
int confirm_dung_pee();

int main()
{

    confirm_dung_pee();

    return 0;
}

void dummy(int, void*) {

}


void set_color()
{
     // 응가
    int hue = 4;
    int saturation = 69;
    int value = 28;

    // 쉬
	int hue2 = 50;
    int saturation2 = 15;
    int value2 = 20;

    lower = Vec3b(hue2 - 13, threshold2 + 10, threshold2 + 10);
    upper = Vec3b(200, 124, 98);

    cout << "hue = " << hue << endl;
    cout << "saturation = " << saturation << endl;
    cout << "value = " << value << endl;

    cout << "case 1" << endl;
    lower_blue1 = Vec3b(hue, threshold1 + 20, threshold1);

	upper_blue1 = Vec3b(130, 130, 130);
    lower_blue2 = Vec3b(0, threshold1 + 20, threshold1);

	upper_blue2 = Vec3b(hue, 130, 130);
    lower_blue3 = Vec3b(hue, threshold1 + 20, threshold1);

	upper_blue3 = Vec3b(hue + 10, 130, 130);

    cout << "hue = " << hue << endl;
    cout << "#1 = " << lower_blue1 << "~" << upper_blue1 << endl;
    cout << "#2 = " << lower_blue2 << "~" << upper_blue2 << endl;
    cout << "#3 = " << lower_blue3 << "~" << upper_blue3 << endl;

}

void set_color2()
{
    int hue = 25;
    int saturation = 19;
    int value = 55;

    lower = Vec3b(12, saturation, value);
	upper = Vec3b(100, 130, 130);

    cout << "hue = " << hue << endl;
    cout << "saturation = " << saturation << endl;
    cout << "value = " << value << endl;

    cout << "lower = " << lower << endl;
    cout << "upper = " << upper << endl;

}

void find_white_pixel(Mat gray) {

    Mat bw;
    threshold(gray, bw, 40, 255, THRESH_BINARY_INV | THRESH_OTSU);

    // output, locations of non-zero pixels
    vector<Point> black_pixels;   
    findNonZero(bw, black_pixels);

    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;

    // extract only the external blob
    findContours(bw.clone(), contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

    Mat mask = Mat::zeros(bw.size(), CV_8UC1);

    // draw the contours as a solid blob, and create a mask of the cloud
    for (size_t i = 0; i < contours.size(); i++)
        drawContours(mask, contours, i, Scalar(255, 255, 255), FILLED, 8, hierarchy, 0, Point());

    vector<Point> all_pixels;   // output, locations of non-zero pixels
    findNonZero(mask, all_pixels);

    int white_pixels = all_pixels.size() - black_pixels.size();

    if (white_pixels > before_area + 1000) {

        printf("before area is : %d\n", before_area);
        printf("white_pixels are : %d\n", white_pixels);
        before_area = white_pixels;

        arduino.Write(stop, 1);
        arduino.Write(resume, 1);

        feed = feed + 1;

		if (white_pixels > 12000) {
			node.Write(resume, 1);
		}	
        printf("feed times : %d\n", feed);

    }

    cout << "Cloud all white pixels : " << white_pixels << endl;

    imshow("img_mask", gray);
}

int confirm_dung_pee() {

    // video
    //VideoCapture cap("dung_pee.mp4");
	
    // picamera 
    VideoCapture cap(pipeline, CAP_GSTREAMER);

    // normal camera
    //VideoCapture cap(-1, CAP_V4L2);

    int result = 0;

    // if use normal camera, setting resolution
    // cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    // cap.set(CAP_PROP_FRAME_HEIGHT, 1024);

    namedWindow("img_color");
	setMouseCallback("img_color", onMouse, &frame);    

	set_color();
    set_color2();

    Mat img_hsv;

    if (!cap.isOpened()) {
        cout << "카메라를 열 수 없습니다." << endl;
        return -1;
    }

    int check_dung = 0;
    int continu = 0;
    ssize_t r_bytes;

    int play_or_not;

    while (1)
    {

        if (check_dung  % 3 == 0) {
        arduino.Read(output, sizeof(output));
        play_or_not = atoi(output);
        cout << "read : " << play_or_not << endl;
        }

        cap >> frame;

        // If the frame is empty, break immediately
        if (frame.empty()) {
            break;
        }
        
        if (setting_region) {
            setting_range(frame, pt2);
        }
        cvtColor((setting_region == 1 ? pt2 : frame), img_hsv, COLOR_BGR2HSV);

        Mat img_mask1, img_mask2, img_mask3, img_mask4, img_mask, img_mask_;
        inRange(img_hsv, lower_blue1, upper_blue1, img_mask1);
        inRange(img_hsv, lower_blue2, upper_blue2, img_mask2);
        inRange(img_hsv, lower_blue3, upper_blue3, img_mask3);
        inRange(img_hsv, lower, upper, img_mask4);
        img_mask = img_mask1 | img_mask2 | img_mask3 | img_mask4;

        double morph_size = 3;
        Mat element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1),
            Point(morph_size, morph_size));
        morphologyEx(img_mask, img_mask, MORPH_OPEN, element);
        morphologyEx(img_mask, img_mask, MORPH_CLOSE, element);

        Mat img_result;
        bitwise_and(frame, frame, img_result, img_mask);

        Mat img_labels, stats, centroids;
        int numOfLabels = connectedComponentsWithStats(img_mask, img_labels, stats, centroids, 8, CV_32S);

        for (int j = 1; j < numOfLabels; j++) {
            int area = stats.at<int>(j, CC_STAT_AREA);
            int left = stats.at<int>(j, CC_STAT_LEFT);
            int top = stats.at<int>(j, CC_STAT_TOP);
            int width = stats.at<int>(j, CC_STAT_WIDTH);
            int height = stats.at<int>(j, CC_STAT_HEIGHT);

            int centerX = centroids.at<double>(j, 0);
            int centerY = centroids.at<double>(j, 1);

            if (play_or_not > 10) {
                if (area > 600)
                {
                    circle(frame, Point(centerX, centerY), 5, Scalar(255, 0, 0), 1);
                    rectangle(frame, Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 1);
                    putText(frame, to_string(area), Point(left, top), FONT_HERSHEY_PLAIN, 2.0, Scalar(255, 0, 0), 1);
                    if (check_dung > 80) {
                        find_white_pixel(img_mask);
                        check_dung = 0;
                    }
                }
            }
        }

        imshow("img_color", frame);
        imshow("img_result", img_result);

        if (waitKey(1) > 0) {
            break;
        }
        if(play_or_not == 0){

        }else{
            check_dung++;
        }

        play_or_not > 10 ? check_dung++ : play_or_not;
    }

    return 1;
}

int setting_range(Mat& origen, Mat& target) {
    Mat copy_mt = origen.clone();

    polylines(copy_mt, pt_vt, true, Scalar(100, 255, 200), 2);
    fillPoly(copy_mt, pt_vt, Scalar(100, 255, 200));

    for (int i = 0; i < copy_mt.rows; i++) {
        for (int j = 0; j < copy_mt.cols; j++) {
            if (copy_mt.at<Vec3b>(i, j)[0] != 100 || copy_mt.at<Vec3b>(i, j)[1] != 255 || copy_mt.at<Vec3b>(i, j)[2] != 200) {
                for (int k = 0; k < 3; k++) {
                    copy_mt.at<Vec3b>(i, j)[k] = 0;
                }
            }
        }
    }

    origen.copyTo(target, copy_mt);
    return 2;
}

void onMouse(int event, int x, int y, int flags, void* p)
{
    static Point startPt;

    image = *(static_cast<Mat*>(p));

    int control_start = 0;

    switch (event) {
    case EVENT_LBUTTONDOWN:
        cout << "EVENT_LBUTTONDOWN" << endl;
        if (control_start == 0) {
            startPt = Point(x, y);
            control_start = 1;
        }
        else {
            startPt = pt_vt.at(pt_vt.size() - 1);
        }
        pt_vt.push_back(Point(x, y));

        cout << Point(x, y) << endl;
        break;

    case EVENT_RBUTTONDOWN:
        setting_region = 1;

        for (auto itr = pt_vt.begin(); itr != pt_vt.end(); ++itr) {
            cout << *itr << endl;
        }

        setting_range(image, pt2);
        imshow("img", pt2);

        break;

    }
}











