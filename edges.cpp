#include "edges.hpp"

std::vector<std::pair<cv::Vec4i,cv::Vec4i>> get_matched_lines(const cv::Mat& pone, const cv::Mat& ptwo){
    cv::Mat one, two;
    cv::Canny(pone, one, 0, 30, 3);
    cv::Canny(ptwo, two, 0, 30, 3);

    std::vector<cv::Vec4i> one_lines;
    cv::HoughLinesP( one, one_lines, 1, CV_PI/180, 80, 40, 10 );
    std::vector<cv::Vec4i> two_lines;
    cv::HoughLinesP( two, two_lines, 1, CV_PI/180, 80, 40, 10 );

    filter_lines(one_lines);
    filter_lines(two_lines);

    std::vector<std::pair<cv::Vec4i,cv::Vec4i>> line_matches;
    for(size_t i = 0; i < one_lines.size(); i++){
        size_t match_idx = get_match(one_lines[i], two_lines);
        line_matches.push_back( std::make_pair(one_lines[i],two_lines[match_idx]) );
        //two_lines.erase(two_lines.begin() + match); //Erase after
        //Threshold the difference at some point to eliminate noise
    }

    return line_matches;
}


int abs_dist(const cv::Vec4i& a, const cv::Vec4i& b){
    double dist = 0;
    for(int i = 0; i < 4; i++)
        dist += std::abs(a[i] - b[i]);

    return dist;
}

size_t get_match(const cv::Vec4i& toMatch, const std::vector<cv::Vec4i>& list){
    size_t min_index = -1;
    double min_value = 0xFFFF;
    for(size_t i = 0; i < list.size(); i++){
        double d = abs_dist(toMatch, list[i]);
        if(d < min_value){
            min_value = d;
            min_index= i;
        }
    }
    return min_index;
}

void filter_lines(std::vector<cv::Vec4i>& lines, double min){
    auto filter =  [min](cv::Vec4i line){
        const double angle = std::abs( std::atan2(line[1] - line[3], line[0] - line[2]) );
        return angle > min && angle < (CV_PI - min);
    };

    lines.erase( std::remove_if(lines.begin(), lines.end(), filter), lines.end() );
}
