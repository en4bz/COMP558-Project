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

using std::vector;

void
seperate(cv::Vec4i& line, const vector<cv::KeyPoint>& keypoints, vector<cv::KeyPoint>& above, vector<cv::KeyPoint>& below){
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
    const int dx = 10;
    const int dy = 10;
    cv::Mat mask(sz, CV_8UC1, cv::Scalar(0));

    int left = std::max( 0, std::min(line[0],line[2]) - dx );
    int right = std::min( sz.width, std::max(line[0],line[2]) + dx);

    int top = std::max(0, std::min(line[1],line[3]) - dy);
    int bottom = std::min( sz.height, std::max(line[1],line[3]) + dy);

    cv::Rect roi( cv::Point2i(left,top), cv::Point2i(right,bottom));
    mask(roi) = cv::Scalar(200);
    return mask;
}

int slope(cv::Vec4i line, cv::Point2i p){
    const float dy = line[1] - line[3];
    const float dx = line[0] - line[2];
//    std::cout << "line: " << line << " Point: " << p << " A/B?: " << ((dy/dx)*(p.x-line[2]) + line[3]) << std::endl;
    return (dy/dx)*(p.x-line[2]) + line[3];
}

#include <iostream>

int detector::detect(const cv::Mat& imgA, const cv::Mat& imgB, cv::Vec4i lineA, cv::Vec4i lineB) {


    std::vector<cv::KeyPoint> keypointsA, keypointsB;
    cv::Mat descriptorsA, descriptorsB;
    std::vector<cv::DMatch> matches;

    // detect and filter
    const cv::Mat maskA = make_mask(lineA, imgA.size());
    const cv::Mat maskB = make_mask(lineB, imgB.size());

    this->_detector.detect( imgA, keypointsA, maskA);
    this->_detector.detect( imgB, keypointsB, maskB);

    if(keypointsA.size() < 1 || keypointsB.size() < 1)
        return 1;

    std::vector<cv::KeyPoint> aboveA, belowA, aboveB, belowB;

    seperate(lineA, keypointsA, aboveA, belowA);
    seperate(lineB, keypointsB, aboveB, belowB);

    // extract
    this->extractor.compute( imgA, keypointsA, descriptorsA );
    this->extractor.compute( imgB, keypointsB, descriptorsB );

    // match
    this->matcher.match(descriptorsA, descriptorsB, matches);


    int move_a = 0;
    int move_b = 0;

    const float hamming_thresh = 40;
    //Calculate distances between matched points
    for(size_t i = 0; i < matches.size(); i++) {
        if( matches[i].distance < hamming_thresh ){
            cv::Point2i pointA = keypointsA[matches[i].queryIdx].pt;
            cv::Point2i pointB = keypointsB[matches[i].trainIdx].pt;
            cv::Point2i diff = pointA - pointB;
            int yA = slope(lineA, pointA);
            int yB = slope(lineB, pointB);
            if( (pointA.y > yA) != (pointB.y > yB))
                std::cout << "Above Below Mismatch!" << std::endl;
            if(pointA.y > yA && pointB.y > yB){
                move_b += diff.y;
            }
            else{
                move_a += diff.y;
            }
        }
    }

    std::cout << "Move Above: " << move_a << " Move Below: " << move_b << std::endl;

    // Draw matches
    cv::Point2i a(lineA[0],lineA[1]);
    cv::Point2i b(lineA[2],lineA[3]);

    cv::Point2i aa(lineB[0],lineB[1]);
    cv::Point2i bb(lineB[2],lineB[3]);

    //cv::line(imgA,  a, b, 0);
    //cv::line(imgB, aa,bb, 0);

    cv::Mat imgMatch;
    cv::drawMatches(imgA, keypointsA, imgB, keypointsB, matches, imgMatch);
    cv::imshow(this->windowName, imgMatch);

    return 0;
}
