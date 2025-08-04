#include "ImageProcessor.h"
#include <iostream>
#include <stdexcept>

ImageProcessor::ImageProcessor() {
    // 构造函数
}

ImageProcessor::~ImageProcessor() {
    // 析构函数
}

// === 1. Image (图片加载与显示) ===

bool ImageProcessor::loadImage(const std::string& path) {
    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);
    
    if (image.empty()) {
        std::cerr << "无法加载图像: " << path << std::endl;
        return false;
    }
    
    originalImage = image.clone();
    currentImage = image.clone();
    imagePath = path;
    updateDisplayImage();
    
    std::cout << "成功加载图像: " << path << std::endl;
    std::cout << "图像尺寸: " << image.cols << "x" << image.rows << std::endl;
    
    return true;
}

const cv::Mat& ImageProcessor::getCurrentImage() const {
    return currentImage;
}

const cv::Mat& ImageProcessor::getDisplayImage() const {
    return displayImage;
}

void ImageProcessor::resetToOriginal() {
    if (!originalImage.empty()) {
        currentImage = originalImage.clone();
        updateDisplayImage();
        std::cout << "已重置到原始图像" << std::endl;
    }
}

// === 2. Color (彩色图像处理) ===

void ImageProcessor::convertToGrayscale() {
    ensureImageLoaded();
    
    if (currentImage.channels() == 3) {
        cv::Mat grayImage;
        cv::cvtColor(currentImage, grayImage, cv::COLOR_BGR2GRAY);
        // 转换回3通道用于显示
        cv::cvtColor(grayImage, currentImage, cv::COLOR_GRAY2BGR);
        updateDisplayImage();
        std::cout << "已转换为灰度图像" << std::endl;
    } else {
        std::cout << "图像已经是灰度图像" << std::endl;
    }
}

void ImageProcessor::colorSelect(int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max) {
    ensureImageLoaded();
    
    if (currentImage.channels() != 3) {
        std::cout << "颜色选择需要彩色图像" << std::endl;
        return;
    }
    
    // 转换到HSV色彩空间
    cv::Mat hsvImage;
    cv::cvtColor(currentImage, hsvImage, cv::COLOR_BGR2HSV);
    
    // 创建掩码
    cv::Mat mask;
    cv::Scalar lower(hue_min, sat_min, val_min);
    cv::Scalar upper(hue_max, sat_max, val_max);
    cv::inRange(hsvImage, lower, upper, mask);
    
    // 应用掩码
    cv::Mat result = cv::Mat::zeros(currentImage.size(), currentImage.type());
    currentImage.copyTo(result, mask);
    currentImage = result;
    updateDisplayImage();
    
    std::cout << "颜色选择完成 - HSV范围: H(" << hue_min << "-" << hue_max 
              << ") S(" << sat_min << "-" << sat_max 
              << ") V(" << val_min << "-" << val_max << ")" << std::endl;
}

void ImageProcessor::colorCluster(int k) {
    ensureImageLoaded();
    
    if (currentImage.channels() != 3) {
        std::cout << "颜色聚类需要彩色图像" << std::endl;
        return;
    }
    
    currentImage = performKMeans(currentImage, k);
    updateDisplayImage();
    std::cout << "颜色聚类完成 - K=" << k << std::endl;
}

void ImageProcessor::colorDeconvolution(int channel) {
    ensureImageLoaded();
    
    if (currentImage.channels() != 3) {
        std::cout << "颜色反褶积需要彩色图像" << std::endl;
        return;
    }
    
    if (channel < 0 || channel > 2) {
        std::cout << "无效的通道索引，应为0-2" << std::endl;
        return;
    }
    
    std::vector<cv::Mat> channels;
    cv::split(currentImage, channels);
    
    // 创建单通道图像
    cv::Mat singleChannel = channels[channel];
    cv::cvtColor(singleChannel, currentImage, cv::COLOR_GRAY2BGR);
    updateDisplayImage();
    
    std::string channelNames[] = {"蓝色", "绿色", "红色"};
    std::cout << "颜色反褶积完成 - 提取" << channelNames[channel] << "通道" << std::endl;
}

void ImageProcessor::channelOperation(const std::string& operation, double value) {
    ensureImageLoaded();
    
    cv::Mat result;
    
    if (operation == "add") {
        cv::add(currentImage, cv::Scalar::all(value), result);
    } else if (operation == "subtract") {
        cv::subtract(currentImage, cv::Scalar::all(value), result);
    } else if (operation == "multiply") {
        cv::multiply(currentImage, cv::Scalar::all(value), result);
    } else if (operation == "divide") {
        if (value != 0) {
            cv::divide(currentImage, cv::Scalar::all(value), result);
        } else {
            std::cout << "除数不能为0" << std::endl;
            return;
        }
    } else {
        std::cout << "不支持的操作: " << operation << std::endl;
        return;
    }
    
    currentImage = result;
    updateDisplayImage();
    std::cout << "通道操作完成 - " << operation << " " << value << std::endl;
}

// 辅助函数

bool ImageProcessor::hasImage() const {
    return !currentImage.empty();
}

cv::Size ImageProcessor::getImageSize() const {
    if (hasImage()) {
        return currentImage.size();
    }
    return cv::Size(0, 0);
}

void ImageProcessor::updateDisplayImage() {
    if (!currentImage.empty()) {
        displayImage = currentImage.clone();
    }
}

// 私有辅助函数

cv::Mat ImageProcessor::performKMeans(const cv::Mat& image, int k) {
    // 准备数据
    cv::Mat data;
    image.convertTo(data, CV_32F);
    data = data.reshape(1, data.total());
    
    // K-Means聚类
    cv::Mat labels, centers;
    cv::kmeans(data, k, labels, 
               cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 20, 1.0),
               3, cv::KMEANS_PP_CENTERS, centers);
    
    // 重构图像
    centers.convertTo(centers, CV_8U);
    cv::Mat result(image.size(), image.type());
    
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int cluster_idx = labels.at<int>(i * image.cols + j);
            if (image.channels() == 3) {
                result.at<cv::Vec3b>(i, j) = cv::Vec3b(
                    centers.at<uchar>(cluster_idx, 0),
                    centers.at<uchar>(cluster_idx, 1),
                    centers.at<uchar>(cluster_idx, 2)
                );
            } else {
                result.at<uchar>(i, j) = centers.at<uchar>(cluster_idx, 0);
            }
        }
    }
    
    return result;
}

void ImageProcessor::ensureImageLoaded() const {
    if (!hasImage()) {
        throw std::runtime_error("没有加载图像");
    }
}