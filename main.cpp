#include <list>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/timer/timer.hpp>

#include "edges.hpp"
#include "detectors.hpp"
#include "read_frames.hpp"

int main(int argc, char *argv[]){
    cv::VideoCapture cap;
    std::vector<std::string> files;
    if(argc < 2)
        return 1;

    std::string mode(argv[1]);
    std::string file(argv[2]);

    const bool isVideo = mode == "video";

    if( isVideo && argc == 3) // video videoname
        cap.open(argv[2]);
    else if( isVideo && argc == 2) //video
        cap.open(1);
    else if(mode == "image")
        files = read_frames(argv[2]);
    else{
        return 1;
    }

    int wait_time = 10;
    const bool draw_matches = false;
    if(draw_matches){
        cv::namedWindow("Above");
        cv::namedWindow("Below");
    }

    detector above("Above", draw_matches);
    detector below("Below", draw_matches);

    std::list<cv::Mat> img_queue;
    bool ok = true;
    while( ok ){
        boost::timer::auto_cpu_timer profiler;
        cv::Mat frame, grey;
        if(isVideo){
            if( !cap.read(frame) ){ // get a new frame from camera
                break;
            }
            else{
                cv::cvtColor(frame, grey, CV_BGR2GRAY);
            }
        }
        else{
            if(files.size() == 0)
                break;
            else{
                grey = cv::imread(files.back(), CV_LOAD_IMAGE_GRAYSCALE);
                frame = grey;
            }
            files.pop_back();
        }

        img_queue.push_back(grey);

        if(img_queue.size() != 5)
            continue;

        auto matches = get_matched_lines(img_queue.front(),img_queue.back());

        int i = 0;
        for(auto edge : matches){
            const cv::Vec4i line_one = edge.first;
            const cv::Vec4i line_two = edge.second;

            //Get Movement above and below edge
            const double move_a = above.detect(img_queue.front(), img_queue.back(), line_one, line_two, true);
            const double move_b = below.detect(img_queue.front(), img_queue.back(), line_one, line_two, false);
            const double diff = move_a - move_b;

            if(diff >= 10){
                std::cout << "[ " << diff << " ]" << " Edge: " << line_two << std::endl;
                cv::line(frame, cv::Point(line_two[0], line_two[1]), cv::Point2i(line_two[2],line_two[3]), CV_RGB(255,0,0), 3);
            }
            cv::Scalar color = 0;

            if( i == 1)
                color = CV_RGB(255,0,0);
            if( i == 2)
                color = CV_RGB(0,255,0);
            if( i == 3)
                color = CV_RGB(0,0,255);
            cv::line(frame, cv::Point(line_two[0], line_two[1]), cv::Point2i(line_two[2],line_two[3]), color, 1);
            cv::line(frame, cv::Point(line_one[0], line_one[1]), cv::Point2i(line_one[2],line_one[3]), color, 1);
            i++;
        }
        cv::imshow("Edge",frame);
        if(cv::waitKey(wait_time) == ' ')
            ok = false;

        img_queue.pop_front();
    }
    cap.release();
    return 0;
}