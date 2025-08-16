#include "Morphology.h"
#include <iostream>

Morphology::Morphology() {
}

Morphology::~Morphology() {
}

cv::Mat Morphology::getKernel(int kernelType, int kernelSize) {
    int morphShape;
    switch (kernelType) {
        case 0: morphShape = cv::MORPH_RECT; break;
        case 1: morphShape = cv::MORPH_ELLIPSE; break;
        case 2: morphShape = cv::MORPH_CROSS; break;
        default: morphShape = cv::MORPH_ELLIPSE; break;
    }
    return cv::getStructuringElement(morphShape, cv::Size(kernelSize, kernelSize));
}

// 形态学操作算法实现
cv::Mat Morphology::dilate(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::dilate(image, result, kernel);
    
    std::cout << "DEBUG: dilate applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::erode(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::erode(image, result, kernel);
    
    std::cout << "DEBUG: erode applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::opening(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_OPEN, kernel);
    
    std::cout << "DEBUG: opening applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::closing(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_CLOSE, kernel);
    
    std::cout << "DEBUG: closing applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::gradient(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_GRADIENT, kernel);
    
    std::cout << "DEBUG: gradient applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::topHat(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_TOPHAT, kernel);
    
    std::cout << "DEBUG: topHat applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::blackHat(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_BLACKHAT, kernel);
    
    std::cout << "DEBUG: blackHat applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::separateFeatures(const cv::Mat& image, double edgeThreshold, int separationMethod, int kernelSize) {
    cv::Mat result;
    cv::Mat grayImage;

    std::cout << "DEBUG: separateFeatures input - size=" << image.size() << ", channels=" << image.channels() << std::endl;

    // Use input image directly (assuming it's already in the correct format)
    if (image.channels() == 1) {
        grayImage = image.clone();
    } else if (image.channels() == 3) {
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = image.clone();
    }

    cv::Mat edges;

    // Apply edge detection based on separation method
    switch (separationMethod) {
        case 0: // Canny edge detection
            cv::Canny(grayImage, edges, edgeThreshold * 0.5, edgeThreshold, 3);
            break;
        case 1: // Sobel edge detection
            {
                cv::Mat grad_x, grad_y;
                cv::Sobel(grayImage, grad_x, CV_16S, 1, 0, 3);
                cv::Sobel(grayImage, grad_y, CV_16S, 0, 1, 3);
                cv::convertScaleAbs(grad_x, grad_x);
                cv::convertScaleAbs(grad_y, grad_y);
                cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edges);
                cv::threshold(edges, edges, edgeThreshold, 255, cv::THRESH_BINARY);
            }
            break;
        case 2: // Laplacian edge detection
            cv::Laplacian(grayImage, edges, CV_8U, 3);
            cv::threshold(edges, edges, edgeThreshold, 255, cv::THRESH_BINARY);
            break;
        default:
            cv::Canny(grayImage, edges, edgeThreshold * 0.5, edgeThreshold, 3);
            break;
    }

    // Apply morphological operations to separate features
    cv::Mat kernel = getKernel(1, kernelSize); // Use ellipse kernel
    cv::morphologyEx(edges, result, cv::MORPH_CLOSE, kernel);

    // Find contours and draw separated features
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(result, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Create output image
    result = cv::Mat::zeros(image.size(), CV_8UC3);

    // Draw each contour with different colors for separation
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(
            (i * 50) % 255,
            (i * 100) % 255,
            (i * 150) % 255
        );
        cv::drawContours(result, contours, (int)i, color, 2);
        cv::fillPoly(result, contours, color);
    }

    std::cout << "DEBUG: separateFeatures applied with edgeThreshold=" << edgeThreshold
              << ", separationMethod=" << separationMethod << ", kernelSize=" << kernelSize
              << ", found " << contours.size() << " features" << std::endl;
    return result;
}

// 统一的应用函数
cv::Mat Morphology::applyFunction(const cv::Mat& image, MorphologyFunction function, const std::vector<double>& params) {
    int kernelSize = params.size() > 0 ? (int)params[0] : 5;
    int kernelType = params.size() > 1 ? (int)params[1] : 1; // Default to ELLIPSE
    
    switch (function) {
        case MorphologyFunction::DILATE:
            return dilate(image, kernelSize, kernelType);
        case MorphologyFunction::ERODE:
            return erode(image, kernelSize, kernelType);
        case MorphologyFunction::OPENING:
            return opening(image, kernelSize, kernelType);
        case MorphologyFunction::CLOSING:
            return closing(image, kernelSize, kernelType);
        case MorphologyFunction::GRADIENT:
            return gradient(image, kernelSize, kernelType);
        case MorphologyFunction::TOP_HAT:
            return topHat(image, kernelSize, kernelType);
        case MorphologyFunction::BLACK_HAT:
            return blackHat(image, kernelSize, kernelType);
        case MorphologyFunction::SEPARATE_FEATURES:
            {
                double edgeThreshold = params.size() > 2 ? params[2] : 100.0;
                int separationMethod = params.size() > 3 ? (int)params[3] : 0;
                return separateFeatures(image, edgeThreshold, separationMethod, kernelSize);
            }
        default:
            return image.clone();
    }
}
