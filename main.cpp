#include <deque>
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <boost/timer/timer.hpp>

#include "edges.hpp"
#include "detectors.hpp"

int main(int argc, char *argv[]){
    cv::VideoCapture cap;
    if(argc >= 2)
        cap.open(argv[1]);
    else
        cap.open(1);

    cv::namedWindow("scene",0);

    std::deque<cv::Mat> img_queue;
    while( cap.isOpened() ){
        boost::timer::auto_cpu_timer t;
        cv::Mat frame, gray;// edges;
        cap >> frame; // get a new frame from camera
        cv::cvtColor(frame, gray, CV_BGR2GRAY);
        img_queue.push_back(gray);
        if(img_queue.size() != 5 )
            continue;

        auto matches = get_matched_lines(img_queue.front(),img_queue.back());
        //std::cout << matches.size() << std::endl;

        cv::Vec4i line_one = matches[0].first;
        cv::Vec4i line_two = matches[0].second;

        cv::Point2i a(line_one[0],line_one[1]);
        cv::Point2i b(line_one[2],line_one[3]);

        cv::Point2i aa(line_two[0],line_two[1]);
        cv::Point2i bb(line_two[2],line_two[3]);

        cv::line(frame,a,b, CV_RGB(255, 0, 0));
        cv::line(frame,aa,bb, CV_RGB(0, 255, 0));

        detect(img_queue.front(),img_queue.back(),line_one,line_two);

        cv::imshow("scene",frame);
        img_queue.pop_front();
        if(cv::waitKey(10) >= 0)
            break;
    }
    cap.release();
    return 0;
}