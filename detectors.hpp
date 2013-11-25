#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <opencv2/opencv.hpp>

class detector{
    cv::FastFeatureDetector _detector;
    cv::FREAK extractor;
    cv::BFMatcher matcher;
    std::string windowName;
public:
    detector(std::string pname) : _detector(20), matcher(cv::NORM_HAMMING), windowName(pname) {}
    int detect(cv::Mat imgA, cv::Mat imgB, cv::Vec4i lineA, cv::Vec4i lineB);// = cv::Vec4i());
};

#endif