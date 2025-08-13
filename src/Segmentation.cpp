#include "Segmentation.h"
#include <iostream>

Segmentation::Segmentation() {
}

Segmentation::~Segmentation() {
}

// THRESHOLD类别算法实现
cv::Mat Segmentation::basicThreshold(const cv::Mat& image, double threshold, int type) {
    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (image.channels() == 3) {
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = image.clone();
    }
    
    // Apply threshold
    int thresholdType = (type == 0) ? cv::THRESH_BINARY : cv::THRESH_BINARY_INV;
    cv::threshold(grayImage, result, threshold, 255, thresholdType);
    
    // Convert back to 3-channel for consistency
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    std::cout << "DEBUG: basicThreshold applied with threshold=" << threshold << ", type=" << type << std::endl;
    return result;
}

cv::Mat Segmentation::rangeThreshold(const cv::Mat& image, double minVal, double maxVal) {
    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (image.channels() == 3) {
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = image.clone();
    }
    
    // Apply range threshold using inRange
    cv::inRange(grayImage, cv::Scalar(minVal), cv::Scalar(maxVal), result);
    
    // Convert back to 3-channel for consistency
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    std::cout << "DEBUG: rangeThreshold applied with minVal=" << minVal << ", maxVal=" << maxVal << std::endl;
    return result;
}

cv::Mat Segmentation::adaptiveThreshold(const cv::Mat& image, int method, int type, int blockSize, double C) {
    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (image.channels() == 3) {
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = image.clone();
    }
    
    // Ensure block size is odd and >= 3
    if (blockSize % 2 == 0) blockSize++;
    if (blockSize < 3) blockSize = 3;
    
    // Apply adaptive threshold
    int adaptiveMethod = (method == 0) ? cv::ADAPTIVE_THRESH_MEAN_C : cv::ADAPTIVE_THRESH_GAUSSIAN_C;
    int thresholdType = (type == 0) ? cv::THRESH_BINARY : cv::THRESH_BINARY_INV;
    
    cv::adaptiveThreshold(grayImage, result, 255, adaptiveMethod, thresholdType, blockSize, C);
    
    // Convert back to 3-channel for consistency
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    std::cout << "DEBUG: adaptiveThreshold applied with method=" << method << ", type=" << type 
              << ", blockSize=" << blockSize << ", C=" << C << std::endl;
    return result;
}

cv::Mat Segmentation::emThreshold(const cv::Mat& image) {
    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (image.channels() == 3) {
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = image.clone();
    }
    
    // Use Otsu's method (EM-like threshold)
    cv::threshold(grayImage, result, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    
    // Convert back to 3-channel for consistency
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    std::cout << "DEBUG: emThreshold (OTSU) applied" << std::endl;
    return result;
}

cv::Mat Segmentation::localThreshold(const cv::Mat& image, int blockSize, double C) {
    // Use adaptive threshold as a simplified local threshold implementation
    return adaptiveThreshold(image, 1, 0, blockSize, C); // Gaussian method, binary
}

// 统一的应用函数
cv::Mat Segmentation::applyFunction(const cv::Mat& image, SegmentationFunction function, const std::vector<double>& params) {
    switch (function) {
        case SegmentationFunction::BASIC_THRESHOLD:
            return basicThreshold(image, params.size() > 0 ? params[0] : 127.0, params.size() > 1 ? (int)params[1] : 0);
        case SegmentationFunction::RANGE_THRESHOLD:
            return rangeThreshold(image, params.size() > 0 ? params[0] : 50.0, params.size() > 1 ? params[1] : 200.0);
        case SegmentationFunction::ADAPTIVE_THRESHOLD:
            return adaptiveThreshold(image, params.size() > 0 ? (int)params[0] : 0, 
                                   params.size() > 1 ? (int)params[1] : 0,
                                   params.size() > 2 ? (int)params[2] : 11, 
                                   params.size() > 3 ? params[3] : 2.0);
        case SegmentationFunction::EM_THRESHOLD:
            return emThreshold(image);
        case SegmentationFunction::LOCAL_THRESHOLD:
            return localThreshold(image, params.size() > 0 ? (int)params[0] : 11, params.size() > 1 ? params[1] : 2.0);
        default:
            return image.clone();
    }
}
