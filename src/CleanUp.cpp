#include "CleanUp.h"
#include <iostream>

CleanUp::CleanUp() {
}

CleanUp::~CleanUp() {
}

// FILL_HOLES类别算法实现
cv::Mat CleanUp::fillAllHoles(const cv::Mat& image, int minHoleSize) {
    CV_Assert(!image.empty());
    
    cv::Mat binaryImage;
    std::cout << "DEBUG: fillAllHoles starting with image size=" << image.size()
              << ", channels=" << image.channels() << ", minHoleSize=" << minHoleSize << std::endl;

    // 确保输入是单通道二值图像
    if (image.channels() != 1) {
        std::cout << "DEBUG: Using fillAllHoles must be single-channel" << std::endl;
        return image.clone();
    }
    binaryImage = image.clone();

    // 使用改进的孔洞填充方法
    cv::Mat result = fillHolesByContours(binaryImage, minHoleSize);

    std::cout << "DEBUG: fillAllHoles completed, result size=" << result.size() << std::endl;
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

// 私有辅助方法实现 - 智能孔洞填充方法
cv::Mat CleanUp::fillHolesByContours(const cv::Mat& image, int minSize) {
    CV_Assert(image.type() == CV_8UC1);
    
    std::cout << "DEBUG: fillHolesByContours - input image size=" << image.size()
              << ", type=" << image.type() << ", minSize=" << minSize << std::endl;

    // 首先确定图像的背景颜色（通过计算平均亮度）
    cv::Scalar meanVal = cv::mean(image);
    bool isDarkBackground = (meanVal[0] < 128);
    std::cout << "DEBUG: Image mean brightness: " << meanVal[0] << ", isDarkBackground: " << isDarkBackground << std::endl;
    
    // 创建一个用于处理的副本
    cv::Mat binaryImage;
    
    // 根据背景颜色，可能需要反转图像以确保背景为黑色（这样孔洞就是黑色区域）
    if (!isDarkBackground) {
        // 如果背景是亮色的，反转图像使背景变为黑色
        cv::bitwise_not(image, binaryImage);
        std::cout << "DEBUG: Image inverted (bright background -> dark background)" << std::endl;
    } else {
        binaryImage = image.clone();
    }
    
    // 创建结果图像
    cv::Mat result;
    if (!isDarkBackground) {
        // 如果原始背景是亮色的，结果也应该是亮色背景
        result = cv::Mat(image.size(), image.type(), cv::Scalar(255));
    } else {
        // 如果原始背景是暗色的，结果也应该是暗色背景
        result = cv::Mat(image.size(), image.type(), cv::Scalar(0));
    }
    
    // 查找轮廓（在黑色背景上的白色对象）
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    std::cout << "DEBUG: Found " << contours.size() << " contours" << std::endl;
    
    int filledCount = 0;
    // 绘制所有大于最小尺寸的轮廓
    for (int i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area >= minSize) {
            // 根据原始背景颜色决定填充颜色
            cv::Scalar fillColor = isDarkBackground ? cv::Scalar(255) : cv::Scalar(0);
            cv::drawContours(result, contours, i, fillColor, cv::FILLED);
            filledCount++;
        }
    }
    
    // 如果需要，反转结果以匹配原始图像的背景
    if (!isDarkBackground) {
        cv::bitwise_not(result, result);
    }
    
    std::cout << "DEBUG: Filled " << filledCount << " objects out of " << contours.size() << " contours" << std::endl;
    return result;
}

cv::Mat CleanUp::rejectFeaturesBySize(const cv::Mat& image, int minSize, int maxSize) {
    CV_Assert(image.type() == CV_8UC1);
    
    std::cout << "DEBUG: rejectFeaturesBySize - Size range: " << minSize << " - " << maxSize << std::endl;
    
    // 简化方法：假设输入是标准二值图像（背景=0，前景=255）
    // 这样避免复杂的背景检测和图像反转
    cv::Mat workingImage = image.clone();
    
    // 查找轮廓（包括内部孔洞，保持原有结构）
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(workingImage, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    std::cout << "DEBUG: rejectFeaturesBySize - found " << contours.size() << " contours (including holes)" << std::endl;

    // 创建结果图像（黑色背景）
    cv::Mat result = cv::Mat::zeros(image.size(), image.type());
    
    int acceptedCount = 0;
    int rejectedCount = 0;
    
    // 处理外部轮廓（hierarchy[i][3] == -1 表示外部轮廓）
    for (size_t i = 0; i < contours.size(); i++) {
        // 只处理外部轮廓
        if (hierarchy[i][3] == -1) {
            double area = cv::contourArea(contours[i]);
            
            if (area >= minSize && area <= maxSize) {
                // 接受这个特征：先绘制外部轮廓
                cv::drawContours(result, contours, (int)i, cv::Scalar(255), cv::FILLED);
                
                // 然后绘制所有内部孔洞（挖空，恢复孔洞）
                int holeIndex = hierarchy[i][2]; // 第一个孔洞的索引
                while (holeIndex != -1) {
                    cv::drawContours(result, contours, holeIndex, cv::Scalar(0), cv::FILLED);
                    holeIndex = hierarchy[holeIndex][0]; // 下一个孔洞
                }
                
                acceptedCount++;
                std::cout << "DEBUG: Accepted feature " << i << " with area=" << area << " pixels" << std::endl;
            } else {
                rejectedCount++;
                std::cout << "DEBUG: Rejected feature " << i << " with area=" << area << " pixels (outside range " 
                          << minSize << "-" << maxSize << ")" << std::endl;
            }
        }
    }

    std::cout << "DEBUG: Summary - Accepted: " << acceptedCount << ", Rejected: " << rejectedCount 
              << ", Total: " << (acceptedCount + rejectedCount) << " features" << std::endl;
    
    return result;
}

// 统一的应用函数
cv::Mat CleanUp::applyFunction(const cv::Mat& image, CleanUpFunction function, const std::vector<double>& params) {
    switch (function) {
        case CleanUpFunction::FILL_ALL_HOLES:
            return fillAllHoles(image, 
                              params.size() > 0 ? (int)params[0] : 50);
        case CleanUpFunction::REJECT_FEATURES:
            return rejectFeatures(image, 
                                params.size() > 0 ? (int)params[0] : 10, 
                                params.size() > 1 ? (int)params[1] : 1000,
                                params.size() > 2 ? (int)params[2] : 0);
        default:
            return image.clone();
    }
}
