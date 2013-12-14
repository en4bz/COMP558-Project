#include "detectors.hpp"

void distance_filter(cv::Vec4i line, std::vector<cv::KeyPoint>& keypoints){
    auto filter = [&line](cv::KeyPoint kp){
        const int max_distance = 25;
        const int mid = (line[1]+line[3])/2;
        return kp.pt.x < line[0] ||  kp.pt.x > line[2] || std::abs(kp.pt.y - mid) > max_distance;
    };

    keypoints.erase( std::remove_if(keypoints.begin(), keypoints.end(), filter), keypoints.end());
}

cv::Mat mask(const cv::Vec4i line, const cv::Size sz, const bool above){
    const int max_dist = 25;
    cv::Mat mask(sz, CV_8UC1, cv::Scalar(0));
    cv::LineIterator lit(mask, cv::Point2d(line[0],line[1]), cv::Point2i(line[2],line[3]) );
    for(int i = 0; i < lit.count; i++){
        cv::Point2i p = lit.pos(); //
        for(int j = 0; j < max_dist; j ++){
            cv::Point2i inc(0,j);
            if(above && (p - inc).y > 0)
                mask.at<uchar>(p - inc) = 200;
            else if( (p + inc).y < sz.height)
                mask.at<uchar>(p + inc) = 200;
            else
                break;
        }
        lit++;
    }
   //cv::imshow("Mask",mask);
   // cv::waitKey(0);
    return mask;
}

int dist_from_line(const cv::Vec4i v, const cv::Point2i p){
    auto dot = [](const cv::Point2i u, const cv::Point2i v ) -> double {
        return u.x*v.x + u.y*v.y;
    };
    const cv::Point2i org(v[0],v[1]);
    const cv::Point2i a = p - org;
    const cv::Point2i b = cv::Point2i(v[2],v[3]) - org;
    const double scalar = dot(a,b) / dot(b,b);
    const cv::Point2i rejection = a - b*scalar;
    const double len = sqrt(dot(rejection,rejection));
 //   std::cout << "Len: " << len << std::endl;
    return len;
}

double detector::detect(const cv::Mat& imgA, const cv::Mat& imgB, const cv::Vec4i lineA, const cv::Vec4i lineB, const bool above) {

    std::vector<cv::KeyPoint> keypointsA, keypointsB;
    cv::Mat descriptorsA, descriptorsB;
    std::vector<cv::DMatch> matches;

    // detect and filter
    const cv::Mat maskA = mask(lineA, imgA.size(), above);
    const cv::Mat maskB = mask(lineB, imgB.size(), above);

    this->mDetector.detect( imgA, keypointsA, maskA);
    this->mDetector.detect( imgB, keypointsB, maskB);

    if(keypointsA.size() < 1 || keypointsB.size() < 1)
        return 0;

    // extract
    this->mExtractor.compute( imgA, keypointsA, descriptorsA );
    this->mExtractor.compute( imgB, keypointsB, descriptorsB );

    // match
    this->mMatcher.match(descriptorsA, descriptorsB, matches);


    //Calculate distances between matched points
    //Verified that below is alway positive and above is always negative.
    const int bin_size = 1;
    const float hamming_thresh = 10;
    std::multiset<int> histogramA;
    std::multiset<int> histogramB;

    int max_d = 0;

    for(size_t i = 0; i < matches.size(); i++) {
        if( matches[i].distance < hamming_thresh ){
            //std::cout << matches[i].distance << " | ";
            const cv::Point2i pointA = keypointsA[matches[i].queryIdx].pt;
            const cv::Point2i pointB = keypointsB[matches[i].trainIdx].pt;
            const int d1 = dist_from_line(lineA, pointA);
            const int d2 = dist_from_line(lineB, pointB);
            max_d = std::max(max_d,d1);
            max_d = std::max(max_d,d2);
            histogramA.insert(d1);
            histogramB.insert(d2);
        }
    }

    //std::cout << "Matches Made: " << histogramA.size() << std::endl;

    int maxA = 0;
    int maxB = 0;
    int max_valueA = -1;
    int max_valueB = -1;

    for(int i = 0; i <= max_d; i++){
        int cA = histogramA.count(i); //Number with key i
        int cB = histogramB.count(i); //Number with key i
        if(cA >= max_valueA){
            max_valueA = cA;
            maxA = i;
        }
        if(cB >= max_valueB){
            max_valueB = cB;
            maxB = i;
        }
    }

   // std::cout << maxA << "+" << histogramA.count(maxA) << "|" << maxB << "+" << histogramB.count(maxB) << std::endl;


    if(this->mDraw){
        cv::Mat imgMatch;
        cv::drawMatches(imgA, keypointsA, imgB, keypointsB, matches, imgMatch);
        cv::imshow(this->windowName, imgMatch);
    }
    return std::abs(maxA - maxB);
}