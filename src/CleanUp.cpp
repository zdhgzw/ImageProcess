#include "CleanUp.h"
#include <iostream>

CleanUp::CleanUp() {
}

CleanUp::~CleanUp() {
}

// REJECT类别算法实现
cv::Mat CleanUp::fillAllHoles(const cv::Mat& image, int minHoleSize, int fillMethod) {
    cv::Mat result;
    cv::Mat binaryImage;
    
    // Convert to binary if needed
    if (image.channels() == 3) {
        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
        cv::threshold(grayImage, binaryImage, 127, 255, cv::THRESH_BINARY);
    } else if (image.type() == CV_8UC1) {
        cv::threshold(image, binaryImage, 127, 255, cv::THRESH_BINARY);
    } else {
        binaryImage = image.clone();
    }
    
    result = findAndFillHoles(binaryImage, minHoleSize);
    
    // Apply different fill methods
    switch (fillMethod) {
        case 0: // Simple fill
            // Already done in findAndFillHoles
            break;
        case 1: // Morphological fill
            {
                cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
                cv::morphologyEx(result, result, cv::MORPH_CLOSE, kernel);
            }
            break;
        case 2: // Flood fill
            {
                cv::Mat temp = result.clone();
                cv::floodFill(temp, cv::Point(0, 0), cv::Scalar(255));
                cv::bitwise_not(temp, temp);
                cv::bitwise_or(result, temp, result);
            }
            break;
        default:
            break;
    }
    
    // Convert back to 3-channel for consistency
    if (result.channels() == 1 && image.channels() == 3) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    std::cout << "DEBUG: fillAllHoles applied with minHoleSize=" << minHoleSize 
              << ", fillMethod=" << fillMethod << std::endl;
    return result;
}

cv::Mat CleanUp::rejectFeatures(const cv::Mat& image, int minFeatureSize, int maxFeatureSize, int rejectMethod) {
    cv::Mat result;
    cv::Mat binaryImage;
    
    // Convert to binary if needed
    if (image.channels() == 3) {
        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
        cv::threshold(grayImage, binaryImage, 127, 255, cv::THRESH_BINARY);
    } else if (image.type() == CV_8UC1) {
        cv::threshold(image, binaryImage, 127, 255, cv::THRESH_BINARY);
    } else {
        binaryImage = image.clone();
    }
    
    switch (rejectMethod) {
        case 0: // Size-based rejection
            result = rejectFeaturesBySize(binaryImage, minFeatureSize, maxFeatureSize);
            break;
        case 1: // Area-based rejection with morphology
            {
                result = rejectFeaturesBySize(binaryImage, minFeatureSize, maxFeatureSize);
                cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
                cv::morphologyEx(result, result, cv::MORPH_OPEN, kernel);
            }
            break;
        case 2: // Contour-based rejection
            {
                std::vector<std::vector<cv::Point>> contours;
                cv::findContours(binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                
                result = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
                
                for (size_t i = 0; i < contours.size(); i++) {
                    double area = cv::contourArea(contours[i]);
                    if (area >= minFeatureSize && area <= maxFeatureSize) {
                        cv::drawContours(result, contours, (int)i, cv::Scalar(255), -1);
                    }
                }
            }
            break;
        default:
            result = rejectFeaturesBySize(binaryImage, minFeatureSize, maxFeatureSize);
            break;
    }
    
    // Convert back to 3-channel for consistency
    if (result.channels() == 1 && image.channels() == 3) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    std::cout << "DEBUG: rejectFeatures applied with minFeatureSize=" << minFeatureSize 
              << ", maxFeatureSize=" << maxFeatureSize << ", rejectMethod=" << rejectMethod << std::endl;
    return result;
}

// 私有辅助方法实现
cv::Mat CleanUp::findAndFillHoles(const cv::Mat& image, int minSize) {
    cv::Mat result = image.clone();
    cv::Mat inverted;
    cv::bitwise_not(result, inverted);
    
    // Find contours of holes (inverted image)
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(inverted, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Fill holes larger than minSize
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area >= minSize) {
            cv::drawContours(result, contours, (int)i, cv::Scalar(255), -1);
        }
    }
    
    return result;
}

cv::Mat CleanUp::rejectFeaturesBySize(const cv::Mat& image, int minSize, int maxSize) {
    cv::Mat result = cv::Mat::zeros(image.size(), CV_8UC1);
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Keep only features within size range
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area >= minSize && area <= maxSize) {
            cv::drawContours(result, contours, (int)i, cv::Scalar(255), -1);
        }
    }
    
    return result;
}

// 统一的应用函数
cv::Mat CleanUp::applyFunction(const cv::Mat& image, CleanUpFunction function, const std::vector<double>& params) {
    switch (function) {
        case CleanUpFunction::FILL_ALL_HOLES:
            return fillAllHoles(image, 
                              params.size() > 0 ? (int)params[0] : 50, 
                              params.size() > 1 ? (int)params[1] : 0);
        case CleanUpFunction::REJECT_FEATURES:
            return rejectFeatures(image, 
                                params.size() > 0 ? (int)params[0] : 10, 
                                params.size() > 1 ? (int)params[1] : 1000,
                                params.size() > 2 ? (int)params[2] : 0);
        default:
            return image.clone();
    }
}
