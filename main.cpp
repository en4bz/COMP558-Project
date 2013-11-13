#include <vector>
#include <iostream>
#include <algorithm>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

void filter_lines(std::vector<cv::Vec4i>& lines, double min = M_PI/6){

    auto filter =  [min](cv::Vec4i line){
        const double angle = std::abs(std::atan2(line[1] - line[3], line[0] - line[2]));
        std::cout << angle << std::endl;
        return angle > min && angle < (M_PI - min);
    };

    //std::cout << "b: " << lines.size() << std::endl;
    lines.erase( std::remove_if(lines.begin(), lines.end(), filter), lines.end());
    //std::cout << "a: " << lines.size() << std::endl;
}

int main(int argc, char *argv[]){
    cv::VideoCapture cap(1);
    if(argc == 2)
        cap.open(argv[1]);

    cv::namedWindow("edges",0);
    cv::namedWindow("lines",0);
    for(;;){
        cv::Mat frame, edges;
        cap >> frame; // get a new frame from camera
        cv::cvtColor(frame, edges, CV_BGR2GRAY);
        cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
        cv::Canny(edges, edges, 0, 30, 3);

        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP( edges, lines, 1, CV_PI/180, 80, 40, 10 );
        filter_lines(lines);
        cv::Mat line_im = cv::Mat::zeros(edges.size(),0);
        for( size_t i = 0; i < lines.size(); i++ ){
            cv::line( line_im, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), 255, 2, 8 );
        }
        cv::imshow("edges", edges);
        cv::imshow("lines", line_im);
        if(cv::waitKey(30) >= 0)
            break;
    }
    return 0;
}