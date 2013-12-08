#include <list>
#include <vector>
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

    cv::namedWindow("scene");

    detector d("scene");
    std::list<cv::Mat> img_queue;
    while( cap.isOpened() ){
        boost::timer::auto_cpu_timer t;
        cv::Mat frame, gray;// edges;
        cap >> frame; // get a new frame from camera
        cv::cvtColor(frame, gray, CV_BGR2GRAY);
        img_queue.push_back(gray);
        if(img_queue.size() != 5 )
            continue;

        auto matches = get_matched_lines(img_queue.front(),img_queue.back());

        cv::Vec4i line_one = matches[0].first;
        cv::Vec4i line_two = matches[0].second;

        d.detect(img_queue.front(),img_queue.back(),line_one,line_two);

        img_queue.pop_front();
        if(cv::waitKey(100) > 0)
            break;
    }
    cap.release();
    return 0;
}