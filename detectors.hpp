#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <opencv2/opencv.hpp>

int detect(cv::Mat imgA, cv::Mat imgB, cv::Vec4i lineA, cv::Vec4i lineB);// = cv::Vec4i());

#endif