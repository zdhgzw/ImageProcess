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

    std::cout << "DEBUG: fillAllHoles starting with image size=" << image.size()
              << ", channels=" << image.channels() << ", minHoleSize=" << minHoleSize
              << ", fillMethod=" << fillMethod << std::endl;

    // Use input image directly (assuming it's already processed by Segmentation)
    if (image.channels() == 1) {
        binaryImage = image.clone();
        std::cout << "DEBUG: Using single-channel input image directly" << std::endl;
    } else if (image.channels() == 3) {
        // Convert to grayscale but don't threshold (assume it's already processed)
        cv::cvtColor(image, binaryImage, cv::COLOR_BGR2GRAY);
        std::cout << "DEBUG: Converted 3-channel to single-channel (no thresholding)" << std::endl;
    } else {
        binaryImage = image.clone();
        std::cout << "DEBUG: Using image as-is" << std::endl;
    }

    std::cout << "DEBUG: Binary image created, size=" << binaryImage.size() << std::endl;

    // Apply different fill methods
    switch (fillMethod) {
        case 0: // Simple contour-based fill
            result = findAndFillHoles(binaryImage, minHoleSize);
            break;
        case 1: // Morphological fill
            {
                result = binaryImage.clone();
                cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
                cv::morphologyEx(result, result, cv::MORPH_CLOSE, kernel);
                // Then apply contour-based fill for remaining holes
                result = findAndFillHoles(result, minHoleSize);
            }
            break;
        case 2: // Flood fill from borders
            {
                result = binaryImage.clone();
                cv::Mat temp = result.clone();

                // Add border to ensure flood fill works from edges
                cv::copyMakeBorder(temp, temp, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));

                // Flood fill from all border points
                cv::floodFill(temp, cv::Point(0, 0), cv::Scalar(255));

                // Remove border and invert to get holes
                cv::Rect roi(1, 1, result.cols, result.rows);
                temp = temp(roi);
                cv::bitwise_not(temp, temp);

                // Combine with original to fill holes
                cv::bitwise_or(result, temp, result);
            }
            break;
        default:
            result = findAndFillHoles(binaryImage, minHoleSize);
            break;
    }

    // Keep result in same format as processing (single channel for binary images)

    std::cout << "DEBUG: fillAllHoles completed, result size=" << result.size()
              << ", channels=" << result.channels() << std::endl;
    return result;
}

cv::Mat CleanUp::rejectFeatures(const cv::Mat& image, int minFeatureSize, int maxFeatureSize, int rejectMethod) {
    cv::Mat result;
    cv::Mat binaryImage;

    std::cout << "DEBUG: rejectFeatures starting with image size=" << image.size()
              << ", channels=" << image.channels() << ", minFeatureSize=" << minFeatureSize
              << ", maxFeatureSize=" << maxFeatureSize << ", rejectMethod=" << rejectMethod << std::endl;

    // Use input image directly (assuming it's already processed by Segmentation)
    if (image.channels() == 1) {
        binaryImage = image.clone();
        std::cout << "DEBUG: Using single-channel input image directly" << std::endl;
    } else if (image.channels() == 3) {
        // Convert to grayscale but don't threshold (assume it's already processed)
        cv::cvtColor(image, binaryImage, cv::COLOR_BGR2GRAY);
        std::cout << "DEBUG: Converted 3-channel to single-channel (no thresholding)" << std::endl;
    } else {
        binaryImage = image.clone();
        std::cout << "DEBUG: Using image as-is" << std::endl;
    }

    std::cout << "DEBUG: Binary image created for rejectFeatures" << std::endl;

    switch (rejectMethod) {
        case 0: // Size-based rejection
            result = rejectFeaturesBySize(binaryImage, minFeatureSize, maxFeatureSize);
            break;
        case 1: // Area-based rejection with morphology
            {
                result = rejectFeaturesBySize(binaryImage, minFeatureSize, maxFeatureSize);
                cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
                cv::morphologyEx(result, result, cv::MORPH_OPEN, kernel);
                std::cout << "DEBUG: Applied morphological opening after size rejection" << std::endl;
            }
            break;
        case 2: // Contour-based rejection with shape analysis
            {
                std::vector<std::vector<cv::Point>> contours;
                cv::findContours(binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                result = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
                int acceptedCount = 0;

                for (size_t i = 0; i < contours.size(); i++) {
                    double area = cv::contourArea(contours[i]);
                    double perimeter = cv::arcLength(contours[i], true);
                    double circularity = (perimeter > 0) ? (4 * CV_PI * area) / (perimeter * perimeter) : 0;

                    // Accept features based on size and shape
                    if (area >= minFeatureSize && area <= maxFeatureSize && circularity > 0.1) {
                        cv::drawContours(result, contours, (int)i, cv::Scalar(255), -1);
                        acceptedCount++;
                    }
                }
                std::cout << "DEBUG: Contour-based rejection: " << acceptedCount << " features accepted out of " << contours.size() << std::endl;
            }
            break;
        default:
            result = rejectFeaturesBySize(binaryImage, minFeatureSize, maxFeatureSize);
            break;
    }

    // Keep result in same format as processing (single channel for binary images)

    std::cout << "DEBUG: rejectFeatures completed, result size=" << result.size()
              << ", channels=" << result.channels() << std::endl;
    return result;
}

// 私有辅助方法实现
cv::Mat CleanUp::findAndFillHoles(const cv::Mat& image, int minSize) {
    cv::Mat result = image.clone();

    std::cout << "DEBUG: findAndFillHoles - input image size=" << image.size()
              << ", type=" << image.type() << ", minSize=" << minSize << std::endl;

    // Method 1: Use flood fill from borders to find holes
    cv::Mat temp = result.clone();
    cv::Mat mask = cv::Mat::zeros(temp.rows + 2, temp.cols + 2, CV_8UC1);

    // Flood fill from all border pixels to mark external background
    cv::floodFill(temp, mask, cv::Point(0, 0), cv::Scalar(128), 0, cv::Scalar(10), cv::Scalar(10), 4);

    // Find holes (areas that are black but not connected to border)
    cv::Mat holes;
    cv::inRange(result, cv::Scalar(0), cv::Scalar(50), holes); // Find black areas
    cv::Mat external;
    cv::inRange(temp, cv::Scalar(120), cv::Scalar(135), external); // Find flood-filled areas
    cv::bitwise_and(holes, ~external, holes); // Holes = black areas NOT connected to border

    // Find contours of holes
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(holes, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::cout << "DEBUG: Found " << contours.size() << " potential holes using flood fill method" << std::endl;

    int filledCount = 0;
    // Fill holes larger than minSize
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area >= minSize) {
            // Fill the hole by drawing white on the original image
            cv::drawContours(result, contours, (int)i, cv::Scalar(255), -1);
            filledCount++;
            std::cout << "DEBUG: Filled hole " << i << " with area=" << area << std::endl;
        } else {
            std::cout << "DEBUG: Skipped small hole " << i << " with area=" << area << " (< " << minSize << ")" << std::endl;
        }
    }

    std::cout << "DEBUG: Filled " << filledCount << " holes out of " << contours.size() << " candidates" << std::endl;
    return result;
}

cv::Mat CleanUp::rejectFeaturesBySize(const cv::Mat& image, int minSize, int maxSize) {
    cv::Mat result = cv::Mat::zeros(image.size(), CV_8UC1);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::cout << "DEBUG: rejectFeaturesBySize - found " << contours.size() << " features" << std::endl;
    std::cout << "DEBUG: Size range: " << minSize << " - " << maxSize << std::endl;

    int acceptedCount = 0;
    // Keep only features within size range
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area >= minSize && area <= maxSize) {
            cv::drawContours(result, contours, (int)i, cv::Scalar(255), -1);
            acceptedCount++;
            std::cout << "DEBUG: Accepted feature " << i << " with area=" << area << std::endl;
        } else {
            std::cout << "DEBUG: Rejected feature " << i << " with area=" << area << std::endl;
        }
    }

    std::cout << "DEBUG: Accepted " << acceptedCount << " features out of " << contours.size() << std::endl;
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
