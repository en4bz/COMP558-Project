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

void distance_filter(cv::Vec4i line, std::vector<cv::KeyPoint> keypoints){
    const int max_distance = 10;
    auto filter = [=](cv::KeyPoint kp){
        double slope = (line[3] - line[1]) / (double)(line[2] - line[0]);
        int y = slope*kp.pt.x + line[1];
        //std::cout << "y: " << y << "|" << slope << "|" << kp.pt << "|" << line << std::endl;
        return std::abs(kp.pt.y - y) > max_distance;
    };
    keypoints.erase( std::remove_if(keypoints.begin(), keypoints.end(), filter), keypoints.end());
}

int detect(cv::Mat imgA, cv::Mat imgB, cv::Vec4i lineA, cv::Vec4i lineB) {

    std::vector<cv::KeyPoint> keypointsA, keypointsB;
    cv::Mat descriptorsA, descriptorsB;
    std::vector<cv::DMatch> matches;

    // DETECTOR
    cv::FastFeatureDetector detector;//(2000,4);

    // DESCRIPTOR
    cv::FREAK extractor;

    // MATCHER
    cv::BFMatcher matcher(cv::NORM_HAMMING);

    // detect
    detector.detect( imgA, keypointsA );
    detector.detect( imgB, keypointsB );

    //Fitler
    distance_filter(lineA, keypointsA);
    distance_filter(lineB, keypointsB);

    // extract
    extractor.compute( imgA, keypointsA, descriptorsA );
    extractor.compute( imgB, keypointsB, descriptorsB );

    // match
    matcher.match(descriptorsA, descriptorsB, matches);

    std::sort(matches.begin(),matches.end());
    matches.erase(matches.begin()+5, matches.end());

//    for(cv::KeyPoint x : keypointsA)
//        std::cout << x.pt << std::endl;

//    for(auto& x : matches)
//        std::cout << x.imgIdx << "|" << x.queryIdx << "|" << x.trainIdx << std::endl;

    // Draw matches
    cv::Mat imgMatch;
    cv::drawMatches(imgA, keypointsA, imgB, keypointsB, matches, imgMatch);

    cv::namedWindow("matches", 0);
    cv::imshow("matches", imgMatch);
    return 0;
}
