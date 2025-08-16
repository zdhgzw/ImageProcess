#include "Measurements.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Measurements::Measurements() {
}

Measurements::~Measurements() {
}

// PER IMAGE类别算法实现
MeasurementResult Measurements::countObjects(const cv::Mat& image, int minSize, int maxSize, double sensitivity) {
    std::vector<std::vector<cv::Point>> contours = detectObjects(image, minSize, maxSize, sensitivity);
    MeasurementResult result = calculateStatistics(contours);
    
    std::cout << "DEBUG: countObjects found " << result.objectCount << " objects with sensitivity=" 
              << sensitivity << ", minSize=" << minSize << ", maxSize=" << maxSize << std::endl;
    
    return result;
}

// 私有辅助方法实现
std::vector<std::vector<cv::Point>> Measurements::detectObjects(const cv::Mat& image, int minSize, int maxSize, double sensitivity) {
    cv::Mat processedImage;

    std::cout << "DEBUG: detectObjects input - size=" << image.size() << ", channels=" << image.channels() << std::endl;

    // Use input image directly (assuming it's already processed by previous steps)
    if (image.channels() == 1) {
        processedImage = image.clone();
        std::cout << "DEBUG: Using single-channel input directly for object detection" << std::endl;
    } else if (image.channels() == 3) {
        // Convert to grayscale but don't apply additional processing
        cv::cvtColor(image, processedImage, cv::COLOR_BGR2GRAY);
        std::cout << "DEBUG: Converted to grayscale for object detection" << std::endl;
    } else {
        processedImage = image.clone();
    }

    // Apply minimal morphological operations based on sensitivity (only for noise reduction)
    if (sensitivity < 0.8) {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::morphologyEx(processedImage, processedImage, cv::MORPH_OPEN, kernel);
        std::cout << "DEBUG: Applied minimal morphological opening for noise reduction" << std::endl;
    }
    
    // Find contours
    std::vector<std::vector<cv::Point>> allContours;
    cv::findContours(processedImage, allContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Filter contours by size
    std::vector<std::vector<cv::Point>> filteredContours;
    for (const auto& contour : allContours) {
        double area = cv::contourArea(contour);
        if (area >= minSize && area <= maxSize) {
            filteredContours.push_back(contour);
        }
    }
    
    return filteredContours;
}

MeasurementResult Measurements::calculateStatistics(const std::vector<std::vector<cv::Point>>& contours) {
    MeasurementResult result;
    
    result.objectCount = static_cast<int>(contours.size());
    
    if (contours.empty()) {
        result.details = "No objects detected";
        return result;
    }
    
    double totalArea = 0.0;
    double minArea = std::numeric_limits<double>::max();
    double maxArea = 0.0;
    
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        totalArea += area;
        minArea = std::min(minArea, area);
        maxArea = std::max(maxArea, area);
    }
    
    result.totalArea = totalArea;
    result.averageSize = totalArea / contours.size();
    
    // Create detailed statistics string
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Objects: " << result.objectCount << "\n";
    ss << "Total Area: " << result.totalArea << " px²\n";
    ss << "Average Size: " << result.averageSize << " px²\n";
    ss << "Size Range: " << minArea << " - " << maxArea << " px²";
    
    result.details = ss.str();
    
    return result;
}

cv::Mat Measurements::createVisualizationImage(const cv::Mat& image, const MeasurementResult& result, int minSize, int maxSize) {
    cv::Mat visualization = image.clone();
    
    // Detect objects again for visualization
    std::vector<std::vector<cv::Point>> contours = detectObjects(image, minSize, maxSize, 0.5);
    
    // Draw contours with different colors
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(
            (i * 50) % 255,
            (i * 100 + 50) % 255,
            (i * 150 + 100) % 255
        );
        
        // Draw contour
        cv::drawContours(visualization, contours, static_cast<int>(i), color, 2);
        
        // Draw object number
        cv::Moments moments = cv::moments(contours[i]);
        if (moments.m00 != 0) {
            cv::Point2f center(moments.m10 / moments.m00, moments.m01 / moments.m00);
            cv::putText(visualization, std::to_string(i + 1), center, 
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        }
    }
    
    // Add result text overlay
    int textY = 30;
    cv::putText(visualization, "Object Count: " + std::to_string(result.objectCount), 
               cv::Point(10, textY), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    
    textY += 30;
    cv::putText(visualization, "Total Area: " + std::to_string(static_cast<int>(result.totalArea)) + " px²", 
               cv::Point(10, textY), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    
    textY += 30;
    cv::putText(visualization, "Avg Size: " + std::to_string(static_cast<int>(result.averageSize)) + " px²", 
               cv::Point(10, textY), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    
    return visualization;
}

// 统一的应用函数
MeasurementResult Measurements::applyFunction(const cv::Mat& image, MeasurementsFunction function, const std::vector<double>& params) {
    switch (function) {
        case MeasurementsFunction::COUNT:
            return countObjects(image, 
                              params.size() > 0 ? static_cast<int>(params[0]) : 10, 
                              params.size() > 1 ? static_cast<int>(params[1]) : 10000,
                              params.size() > 2 ? params[2] : 0.5);
        default:
            return MeasurementResult();
    }
}
