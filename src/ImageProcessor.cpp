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
        std::cerr << "Failed to load image: " << path << std::endl;
        return false;
    }
    
    originalImage = image.clone();
    currentImage = image.clone();
    imagePath = path;
    updateDisplayImage();
    
    std::cout << "Successfully loaded image: " << path << std::endl;
    std::cout << "Image size: " << image.cols << "x" << image.rows << std::endl;
    
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
        std::cout << "Reset to original image" << std::endl;
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
        std::cout << "Converted to grayscale image" << std::endl;
    } else {
        std::cout << "Image is already grayscale" << std::endl;
    }
}

void ImageProcessor::colorSelect(int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max) {
    ensureImageLoaded();
    
    if (currentImage.channels() != 3) {
        std::cout << "Color selection requires color image" << std::endl;
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
    
    std::cout << "Color selection completed - HSV range: H(" << hue_min << "-" << hue_max 
              << ") S(" << sat_min << "-" << sat_max 
              << ") V(" << val_min << "-" << val_max << ")" << std::endl;
}

void ImageProcessor::colorCluster(int k) {
    ensureImageLoaded();
    
    if (currentImage.channels() != 3) {
        std::cout << "Color clustering requires color image" << std::endl;
        return;
    }
    
    currentImage = performKMeans(currentImage, k);
    updateDisplayImage();
    std::cout << "Color clustering completed - K=" << k << std::endl;
}

void ImageProcessor::colorDeconvolution(int channel) {
    ensureImageLoaded();
    
    if (currentImage.channels() != 3) {
        std::cout << "Color deconvolution requires color image" << std::endl;
        return;
    }
    
    if (channel < 0 || channel > 2) {
        std::cout << "Invalid channel index, should be 0-2" << std::endl;
        return;
    }
    
    std::vector<cv::Mat> channels;
    cv::split(currentImage, channels);
    
    // 创建单通道图像
    cv::Mat singleChannel = channels[channel];
    cv::cvtColor(singleChannel, currentImage, cv::COLOR_GRAY2BGR);
    updateDisplayImage();
    
    std::string channelNames[] = {"Blue", "Green", "Red"};
    std::cout << "Color deconvolution completed - extracted " << channelNames[channel] << " channel" << std::endl;
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
            std::cout << "Divisor cannot be zero" << std::endl;
            return;
        }
    } else {
        std::cout << "Unsupported operation: " << operation << std::endl;
        return;
    }
    
    currentImage = result;
    updateDisplayImage();
    std::cout << "Channel operation completed - " << operation << " " << value << std::endl;
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
        std::cout << "DEBUG: updateDisplayImage - currentImage size: " << currentImage.cols << "x" << currentImage.rows << std::endl;
        displayImage = currentImage.clone();
        std::cout << "DEBUG: displayImage updated to size: " << displayImage.cols << "x" << displayImage.rows << std::endl;
    } else {
        std::cout << "DEBUG: updateDisplayImage - currentImage is empty!" << std::endl;
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

void ImageProcessor::setCurrentImage(const cv::Mat& image) {
    if (!image.empty()) {
        std::cout << "DEBUG: setCurrentImage called with image size: " << image.cols << "x" << image.rows << std::endl;
        currentImage = image.clone();
        std::cout << "DEBUG: currentImage updated, calling updateDisplayImage()" << std::endl;
        updateDisplayImage();
        std::cout << "DEBUG: updateDisplayImage() completed" << std::endl;
    } else {
        std::cout << "DEBUG: setCurrentImage called with empty image!" << std::endl;
    }
}

void ImageProcessor::applyPreProcessedImage(const cv::Mat& processedImage) {
    if (!processedImage.empty()) {
        std::cout << "DEBUG: applyPreProcessedImage called with image size: " << processedImage.cols << "x" << processedImage.rows << std::endl;

        // 确保图像数据类型正确
        cv::Mat correctedImage;
        if (processedImage.type() != currentImage.type()) {
            processedImage.convertTo(correctedImage, currentImage.type());
            std::cout << "DEBUG: Image type corrected from " << processedImage.type() << " to " << currentImage.type() << std::endl;
        } else {
            correctedImage = processedImage.clone();
        }

        // 直接替换当前图像
        currentImage = correctedImage;
        std::cout << "DEBUG: currentImage directly replaced, calling updateDisplayImage()" << std::endl;
        updateDisplayImage();
        std::cout << "DEBUG: applyPreProcessedImage completed" << std::endl;
    } else {
        std::cout << "DEBUG: applyPreProcessedImage called with empty image!" << std::endl;
    }
}

// 预处理功能 - 直接模仿其他工作功能的模式
void ImageProcessor::adjustContrast(double brightness, double contrast) {
    ensureImageLoaded();
    std::cout << "DEBUG: adjustContrast called with brightness=" << brightness << ", contrast=" << contrast << std::endl;

    cv::Mat result;
    currentImage.convertTo(result, currentImage.type(), contrast, brightness);
    currentImage = result;
    updateDisplayImage();

    std::cout << "DEBUG: adjustContrast completed, image updated" << std::endl;
}

void ImageProcessor::applyHistogramEqualization(int method, double clipLimit) {
    ensureImageLoaded();
    std::cout << "DEBUG: applyHistogramEqualization called with method=" << method << ", clipLimit=" << clipLimit << std::endl;

    cv::Mat result;

    if (method == 0) {
        // Global histogram equalization
        if (currentImage.channels() == 1) {
            cv::equalizeHist(currentImage, result);
        } else {
            cv::Mat ycrcb;
            cv::cvtColor(currentImage, ycrcb, cv::COLOR_BGR2YCrCb);
            std::vector<cv::Mat> channels;
            cv::split(ycrcb, channels);
            cv::equalizeHist(channels[0], channels[0]);
            cv::merge(channels, ycrcb);
            cv::cvtColor(ycrcb, result, cv::COLOR_YCrCb2BGR);
        }
    } else {
        // Adaptive histogram equalization (CLAHE)
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clipLimit, cv::Size(8, 8));

        if (currentImage.channels() == 1) {
            clahe->apply(currentImage, result);
        } else {
            cv::Mat ycrcb;
            cv::cvtColor(currentImage, ycrcb, cv::COLOR_BGR2YCrCb);
            std::vector<cv::Mat> channels;
            cv::split(ycrcb, channels);
            clahe->apply(channels[0], channels[0]);
            cv::merge(channels, ycrcb);
            cv::cvtColor(ycrcb, result, cv::COLOR_YCrCb2BGR);
        }
    }

    currentImage = result;
    updateDisplayImage();

    std::cout << "DEBUG: applyHistogramEqualization completed, image updated" << std::endl;
}

void ImageProcessor::flattenBackground(int kernelSize) {
    ensureImageLoaded();
    std::cout << "DEBUG: flattenBackground called with kernelSize=" << kernelSize << std::endl;

    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(currentImage, result, cv::MORPH_TOPHAT, kernel);
    currentImage = result;
    updateDisplayImage();

    std::cout << "DEBUG: flattenBackground completed, image updated" << std::endl;
}

// === 4. Segmentation (图像分割) - THRESHOLD (阈值标记) ===

void ImageProcessor::basicThreshold(double threshold, int type) {
    ensureImageLoaded();
    std::cout << "DEBUG: basicThreshold called with threshold=" << threshold << ", type=" << type << std::endl;

    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (currentImage.channels() == 3) {
        cv::cvtColor(currentImage, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = currentImage.clone();
    }
    
    // Apply threshold
    int thresholdType = (type == 0) ? cv::THRESH_BINARY : cv::THRESH_BINARY_INV;
    cv::threshold(grayImage, result, threshold, 255, thresholdType);
    
    // Convert back to 3-channel for display
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    currentImage = result;
    updateDisplayImage();

    std::cout << "DEBUG: basicThreshold completed, image updated" << std::endl;
}

void ImageProcessor::rangeThreshold(double minVal, double maxVal) {
    ensureImageLoaded();
    std::cout << "DEBUG: rangeThreshold called with min=" << minVal << ", max=" << maxVal << std::endl;

    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (currentImage.channels() == 3) {
        cv::cvtColor(currentImage, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = currentImage.clone();
    }
    
    // Apply range threshold using inRange
    cv::inRange(grayImage, cv::Scalar(minVal), cv::Scalar(maxVal), result);
    
    // Convert back to 3-channel for display
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    currentImage = result;
    updateDisplayImage();

    std::cout << "DEBUG: rangeThreshold completed, image updated" << std::endl;
}

void ImageProcessor::adaptiveThreshold(int method, int type, int blockSize, double C) {
    ensureImageLoaded();
    std::cout << "DEBUG: adaptiveThreshold called with method=" << method << ", type=" << type 
              << ", blockSize=" << blockSize << ", C=" << C << std::endl;

    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (currentImage.channels() == 3) {
        cv::cvtColor(currentImage, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = currentImage.clone();
    }
    
    // Ensure block size is odd and >= 3
    if (blockSize % 2 == 0) blockSize++;
    if (blockSize < 3) blockSize = 3;
    
    // Apply adaptive threshold
    int adaptiveMethod = (method == 0) ? cv::ADAPTIVE_THRESH_MEAN_C : cv::ADAPTIVE_THRESH_GAUSSIAN_C;
    int thresholdType = (type == 0) ? cv::THRESH_BINARY : cv::THRESH_BINARY_INV;
    
    cv::adaptiveThreshold(grayImage, result, 255, adaptiveMethod, thresholdType, blockSize, C);
    
    // Convert back to 3-channel for display
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    currentImage = result;
    updateDisplayImage();

    std::cout << "DEBUG: adaptiveThreshold completed, image updated" << std::endl;
}

void ImageProcessor::emThreshold() {
    ensureImageLoaded();
    std::cout << "DEBUG: emThreshold called (simplified implementation using Otsu's method)" << std::endl;

    cv::Mat result;
    cv::Mat grayImage;
    
    // Convert to grayscale if needed
    if (currentImage.channels() == 3) {
        cv::cvtColor(currentImage, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = currentImage.clone();
    }
    
    // Use Otsu's method as a simplified E-M threshold implementation
    double threshold = cv::threshold(grayImage, result, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    
    std::cout << "DEBUG: Otsu threshold value: " << threshold << std::endl;
    
    // Convert back to 3-channel for display
    if (result.channels() == 1) {
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    }
    
    currentImage = result;
    updateDisplayImage();

    std::cout << "DEBUG: emThreshold completed, image updated" << std::endl;
}

void ImageProcessor::localThreshold() {
    ensureImageLoaded();
    std::cout << "DEBUG: localThreshold called (simplified implementation using adaptive threshold)" << std::endl;

    // Use adaptive threshold as a simplified local threshold implementation
    adaptiveThreshold(1, 0, 11, 2.0); // Gaussian method, binary, block size 11, C=2.0
    
    std::cout << "DEBUG: localThreshold completed using adaptive threshold" << std::endl;
}