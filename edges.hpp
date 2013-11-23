#ifndef EDGES_HPP
#define EDGES_HPP

#include <vector>
#include <utility>
#include <opencv2/opencv.hpp>

/*Takes 2 images offset by some time interval
 *and returns a vector of pairs each of which
 *contain a pair of matches lines from the first
 *image and the second image.
 */
std::vector<std::pair<cv::Vec4i,cv::Vec4i>>
get_matched_lines(const cv::Mat& pone, const cv::Mat& ptwo);

/* Computes distance how close to lines match eachother
 */
int abs_dist(const cv::Vec4i& a, const cv::Vec4i& b);

/*Returns the index of the line that has the closest
 *match relative to toMatch in list
 */
size_t get_match(const cv::Vec4i& toMatch, const std::vector<cv::Vec4i>& list);

/* Filters lines by angle relative to horizontal
 */
void filter_lines(std::vector<cv::Vec4i>& lines, double min = CV_PI/9);

#endif
