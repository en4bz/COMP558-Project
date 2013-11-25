//  demo.cpp
//
//	Here is an example on how to use the descriptor presented in the following paper:
//	A. Alahi, R. Ortiz, and P. Vandergheynst.
//  FREAK: Fast Retina Keypoint. In IEEE Conference on Computer Vision and Pattern Recognition, 2012.
//  CVPR 2012 Open Source Award winner
//
//	Copyright (C) 2011-2012  Signal processing laboratory 2, EPFL,
//	Kirell Benzi (kirell.benzi@epfl.ch),
//	Raphael Ortiz (raphael.ortiz@a3.epfl.ch),
//	Alexandre Alahi (alexandre.alahi@epfl.ch)
//	and Pierre Vandergheynst (pierre.vandergheynst@epfl.ch)
//
//  Redistribution and use in source and binary forms, with or without modification,
//  are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//  This software is provided by the copyright holders and contributors "as is" and
//  any express or implied warranties, including, but not limited to, the implied
//  warranties of merchantability and fitness for a particular purpose are disclaimed.
//  In no event shall the Intel Corporation or contributors be liable for any direct,
//  indirect, incidental, special, exemplary, or consequential damages
//  (including, but not limited to, procurement of substitute goods or services;
//  loss of use, data, or profits; or business interruption) however caused
//  and on any theory of liability, whether in contract, strict liability,
//  or tort (including negligence or otherwise) arising in any way out of
//  the use of this software, even if advised of the possibility of such damage.

#include "detectors.hpp"

void
seperate(cv::Vec4i& line, const std::vector<cv::KeyPoint>& keypoints, std::vector<cv::KeyPoint>& above, std::vector<cv::KeyPoint>& below){
    for(size_t i = 0; i < keypoints.size(); i++){
        cv::Point2i p = keypoints[i].pt;
        if(p.y < line[1] && p.y < line[3])
            below.push_back(keypoints[i]);
        else if(p.y > line[1] && p.y > line[3])
            above.push_back(keypoints[i]);
        else
            continue;//Check using line formula
    }
}

void distance_filter(cv::Vec4i line, std::vector<cv::KeyPoint>& keypoints){
    auto filter = [&line](cv::KeyPoint kp){
        const int max_distance = 25;
        const int mid = (line[1]+line[3])/2;
        return kp.pt.x < line[0] ||  kp.pt.x > line[2] || std::abs(kp.pt.y - mid) > max_distance;
    };

    keypoints.erase( std::remove_if(keypoints.begin(), keypoints.end(), filter), keypoints.end());
}

cv::Mat make_mask(cv::Vec4i line, cv::Size sz){
    const int max_d = 25;
    cv::Mat mask(sz, CV_8UC1, cv::Scalar(0));
    for(int i = std::min(line[1],line[3]) - max_d; i < std::max(line[1],line[3]) + max_d; i++)
        for(int j = line[0]; j < line[2]; j++)
            mask.at<uchar>(i,j) = 200;

//    cv::namedWindow("a",0);
//    cv::imshow("a",mask);
//    cv::waitKey(0);
    return mask;
}

#include <iostream>

int detector::detect(cv::Mat imgA, cv::Mat imgB, cv::Vec4i lineA, cv::Vec4i lineB) {

    std::vector<cv::KeyPoint> keypointsA, keypointsB;
    cv::Mat descriptorsA, descriptorsB;
    std::vector<cv::DMatch> matches;

    // detect and filter
    cv::Mat maskA = make_mask(lineA, imgA.size());
    cv::Mat maskB = make_mask(lineB, imgB.size());

    this->_detector.detect( imgA, keypointsA, maskA);
    this->_detector.detect( imgB, keypointsB, maskB);

    if(keypointsA.size() < 1 || keypointsB.size() < 1)
        return 1;

    std::vector<cv::KeyPoint> aboveA, belowA, aboveB, belowB;

    seperate(lineA, keypointsA, aboveA, belowA);
    seperate(lineA, keypointsB, aboveB, belowB);

    // extract
    this->extractor.compute( imgA, keypointsA, descriptorsA );
    this->extractor.compute( imgB, keypointsB, descriptorsB );

    // match
    this->matcher.match(descriptorsA, descriptorsB, matches);

//    for(int i = 0; i < matches.size(); i++){
//        std::cout << matches[i].trainIdx << std::endl;
//        std::cout << matches[i].queryIdx << std::endl;
//    }

//    for(cv::KeyPoint x : keypointsA)
//        std::cout << x.pt << std::endl;

//    for(auto& x : matches)
//        std::cout << x.imgIdx << "|" << x.queryIdx << "|" << x.trainIdx << std::endl;

    // Draw matches
    cv::Point2i a(lineA[0],lineA[1]);
    cv::Point2i b(lineA[2],lineA[3]);

    cv::Point2i aa(lineB[0],lineB[1]);
    cv::Point2i bb(lineB[2],lineB[3]);

    cv::line(imgA,  a, b, 0);
    cv::line(imgB, aa,bb, 0);

    cv::Mat imgMatch;
    cv::drawMatches(imgA, keypointsA, imgB, keypointsB, matches, imgMatch);

    cv::imshow(this->windowName, imgMatch);
    return 0;
}
