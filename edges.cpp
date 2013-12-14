#include "edges.hpp"

void dilate_one(cv::Mat& grid){
    cv::Size sz = grid.size();
    cv::Mat sc_copy = grid.clone();

    for(int i = 1; i < sz.height -1; i++){
        for(int j = 1; j < sz.width -1; j++){
            if(grid.at<uchar>(i,j) != 0){
                    sc_copy.at<uchar>(i+1,j) = 255;
                    sc_copy.at<uchar>(i-1,j) = 255;
                    sc_copy.at<uchar>(i,j+1) = 255;
                    sc_copy.at<uchar>(i,j-1) = 255;
                    sc_copy.at<uchar>(i-1,j-1) = 255;
                    sc_copy.at<uchar>(i+1,j+1) = 255;
                    sc_copy.at<uchar>(i-1,j+1) = 255;
                    sc_copy.at<uchar>(i+1,j-1) = 255;
            }
        }
    }
    grid = sc_copy;
}

std::vector<std::pair<cv::Vec4i,cv::Vec4i>>
get_matched_lines(const cv::Mat& pone, const cv::Mat& ptwo){
    const double sigma = 3;
    cv::Size kernel_size = cv::Size(7,7);

    cv::Mat one, two;
    cv::GaussianBlur(pone, one, kernel_size, sigma);
    cv::GaussianBlur(ptwo, two, kernel_size, sigma);

    cv::Canny(one, one, 15, 30, 3, true);
    cv::Canny(two, two, 15, 30, 3, true);

    //dilate_one(one);
    //dilate_one(two);
    //cv::imshow("B", one);
    //cv::imshow("C", two);

    const double threshold = 45;
    const double line_length = 50;
    const double line_gap = 20;

    std::vector<cv::Vec4i> one_lines;
    cv::HoughLinesP( one, one_lines, 1, CV_PI/180, threshold, line_length, line_gap );
    std::vector<cv::Vec4i> two_lines;
    cv::HoughLinesP( two, two_lines, 1, CV_PI/180, threshold, line_length, line_gap );

    filter_lines(one_lines);
    filter_lines(two_lines);

    std::vector<std::pair<cv::Vec4i,cv::Vec4i>> line_matches;


    for(size_t i = 0; i < one_lines.size(); i++){
        cv::Vec4i toMatch = one_lines[i];
        auto comp = [toMatch](const cv::Vec4i& a, const cv::Vec4i& b){ return abs_dist(toMatch,a) < abs_dist(toMatch,b); };
        auto it = std::min_element(two_lines.begin(),two_lines.end(),comp);
        int x = std::abs(toMatch[1] - (*it)[1]) + std::abs(toMatch[3]-(*it)[3]); //Make sure difference in y value isn't to much.

        if(x > 30 || toMatch[1] < 300 || toMatch[2] > 350){ //Threshold the difference at some point to eliminate noise
       //     std::cout << x << std::endl;
            continue;
        }
        size_t match_idx = std::distance(two_lines.begin(), it); //min_index;
        line_matches.push_back( std::make_pair(one_lines[i],two_lines[match_idx]) );
    }

    return line_matches;
}


int abs_dist(const cv::Vec4i& a, const cv::Vec4i& b){
    double dist = 0;
    for(int i = 0; i < 4; i++)
        dist += std::abs(a[i] - b[i]);

    return dist;
}

void filter_lines(std::vector<cv::Vec4i>& lines, double min){
    auto filter =  [min](cv::Vec4i line){
        const double angle = std::abs( std::atan2(line[1] - line[3], line[0] - line[2]) );
        return angle > min && angle < (CV_PI - min);
    };

    lines.erase( std::remove_if(lines.begin(), lines.end(), filter), lines.end() );
}
