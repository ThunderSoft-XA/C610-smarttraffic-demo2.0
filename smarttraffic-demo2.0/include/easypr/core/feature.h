#ifndef EASYPR_CORE_FEATURE_H_
#define EASYPR_CORE_FEATURE_H_

#include "opencv2/opencv.hpp"

using namespace cv;

namespace easypr {

//!  Get the characteristic number of license plate 
cv::Mat getHistogram(cv::Mat in);

//!  Getfeatures callback function of easypr 
//!  Training features for generating SVM from license plate image 
typedef void (*svmCallback)(const cv::Mat& image, cv::Mat& features);

//!  Getfeatures callback function of easypr 
//! convert from images to features used by gray char ann
typedef void (*annCallback)(const cv::Mat& image, cv::Mat& features);

//! gray and project feature
void getGrayPlusProject(const cv::Mat& grayChar, cv::Mat& features);

//!   Getfeatures callback function of easypr 
//!  This function is to obtain vertical and horizontal histogram values 
void getHistogramFeatures(const cv::Mat& image, cv::Mat& features);

//!  This function is to obtain sift eigenvectors 
void getSIFTFeatures(const cv::Mat& image, cv::Mat& features);

//!  This function is to obtain hog eigenvectors 
void getHOGFeatures(const cv::Mat& image, cv::Mat& features);

//!  This function is a square graph eigen to obtain the quantization of HSV space 
void getHSVHistFeatures(const cv::Mat& image, cv::Mat& features);

//! LBP feature
void getLBPFeatures(const cv::Mat& image, cv::Mat& features);

//! color feature
void getColorFeatures(const cv::Mat& src, cv::Mat& features);

//! color feature and histom
void getHistomPlusColoFeatures(const cv::Mat& image, cv::Mat& features);

//! get character feature
cv::Mat charFeatures(cv::Mat in, int sizeData);
cv::Mat charFeatures2(cv::Mat in, int sizeData);

//! LBP feature + Histom feature
void getLBPplusHistFeatures(const cv::Mat& image, cv::Mat& features);

//! grayChar feauter
void getGrayCharFeatures(const cv::Mat& grayChar, cv::Mat& features);

void getGrayPlusLBP(const Mat& grayChar, Mat& features);
} /*! \namespace easypr*/

#endif  // EASYPR_CORE_FEATURE_H_