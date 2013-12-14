#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <opencv2/opencv.hpp>

class detector{
    const bool mDraw;
    cv::FastFeatureDetector mDetector;
    cv::BriefDescriptorExtractor mExtractor;
    cv::BFMatcher mMatcher;
    const std::string windowName;
public:
    detector(const std::string& pname, const bool draw) :
        mDraw(draw), mDetector(4), mMatcher(cv::NORM_HAMMING), windowName(pname) {}
    detector(const detector&) = delete; //Non Copyable
    double detect(const cv::Mat& imgA, const cv::Mat& imgB, const cv::Vec4i lineA, const cv::Vec4i lineB, const bool above);
};

#endif