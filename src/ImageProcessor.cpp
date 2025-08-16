#include "ImageProcessor.h"
#include <iostream>

ImageProcessor::ImageProcessor() {
    std::cout << "ImageProcessor initialized" << std::endl;
}

ImageProcessor::~ImageProcessor() {
}

bool ImageProcessor::loadImage(const std::string& path) {
    cv::Mat image = cv::imread(path);
    
    if (image.empty()) {
        std::cout << "Failed to load image: " << path << std::endl;
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

void ImageProcessor::resetToOriginal() {
    if (!originalImage.empty()) {
        currentImage = originalImage.clone();
        updateDisplayImage();
        std::cout << "Image reset to original" << std::endl;
    }
}

const cv::Mat& ImageProcessor::getCurrentImage() const {
    return currentImage;
}

const cv::Mat& ImageProcessor::getDisplayImage() const {
    return displayImage;
}

void ImageProcessor::convertToGrayscale() {
    ensureImageLoaded();

    if (currentImage.channels() == 3) {
        cv::Mat grayImage;
        cv::cvtColor(currentImage, grayImage, cv::COLOR_BGR2GRAY);
        currentImage = grayImage.clone(); // Keep as single channel grayscale
        updateDisplayImage();
        std::cout << "Converted to grayscale (channels=" << currentImage.channels() << ")" << std::endl;
    } else {
        std::cout << "Image is already grayscale (channels=" << currentImage.channels() << ")" << std::endl;
    }
}

void ImageProcessor::colorSelect(int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max) {
    ensureImageLoaded();

    std::cout << "DEBUG: colorSelect input - channels=" << currentImage.channels() << std::endl;

    // Color selection only works on 3-channel images
    if (currentImage.channels() != 3) {
        std::cout << "WARNING: Color selection requires 3-channel image, current has " << currentImage.channels() << " channels" << std::endl;
        return;
    }

    cv::Mat hsv, mask, result;
    cv::cvtColor(currentImage, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(hue_min, sat_min, val_min);
    cv::Scalar upper(hue_max, sat_max, val_max);
    cv::inRange(hsv, lower, upper, mask);

    // Initialize result with zeros and same type as currentImage
    result = cv::Mat::zeros(currentImage.size(), currentImage.type());
    currentImage.copyTo(result, mask);
    currentImage = result;
    updateDisplayImage();

    std::cout << "Applied color selection" << std::endl;
}

void ImageProcessor::colorCluster(int k) {
    ensureImageLoaded();

    std::cout << "DEBUG: colorCluster input - channels=" << currentImage.channels() << std::endl;

    // Color clustering only works on 3-channel images
    if (currentImage.channels() != 3) {
        std::cout << "WARNING: Color clustering requires 3-channel image, current has " << currentImage.channels() << " channels" << std::endl;
        return;
    }

    currentImage = performKMeans(currentImage, k);
    updateDisplayImage();
    std::cout << "Applied K-means clustering with k=" << k << std::endl;
}

void ImageProcessor::colorDeconvolution(int channel) {
    ensureImageLoaded();

    std::cout << "DEBUG: colorDeconvolution input - channels=" << currentImage.channels() << std::endl;

    // Color deconvolution only works on 3-channel images
    if (currentImage.channels() != 3) {
        std::cout << "WARNING: Color deconvolution requires 3-channel image, current has " << currentImage.channels() << " channels" << std::endl;
        return;
    }

    std::vector<cv::Mat> channels;
    cv::split(currentImage, channels);

    if (channel >= 0 && channel < channels.size()) {
        // Extract single channel and keep as grayscale (1-channel)
        currentImage = channels[channel].clone();
        updateDisplayImage();
        std::cout << "Applied color deconvolution on channel " << channel << ", result is grayscale" << std::endl;
    }
}

void ImageProcessor::channelOperation(const std::string& operation, double value) {
    ensureImageLoaded();

    std::cout << "DEBUG: channelOperation input - channels=" << currentImage.channels() << std::endl;

    cv::Mat result;
    if (operation == "add") {
        cv::add(currentImage, cv::Scalar::all(value), result);
    } else if (operation == "subtract") {
        cv::subtract(currentImage, cv::Scalar::all(value), result);
    } else if (operation == "multiply") {
        cv::multiply(currentImage, cv::Scalar::all(value), result);
    } else if (operation == "divide") {
        cv::divide(currentImage, cv::Scalar::all(value), result);
    } else {
        result = currentImage.clone();
    }

    currentImage = result;
    updateDisplayImage();
    std::cout << "Applied channel operation: " << operation << " with value " << value
              << ", result channels=" << currentImage.channels() << std::endl;
}

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
        // Convert to 3-channel for display if needed
        if (currentImage.channels() == 1) {
            // 确保使用正确的转换方式，并创建新的Mat对象而不是直接赋值
            cv::cvtColor(currentImage, displayImage, cv::COLOR_GRAY2BGR);
        } else {
            // 使用clone()创建一个深拷贝，避免共享内存
            displayImage = currentImage.clone();
        }
        std::cout << "DEBUG: updateDisplayImage - currentImage size: " << currentImage.cols << "x" << currentImage.rows
                  << ", channels: " << currentImage.channels() << std::endl;
        std::cout << "DEBUG: displayImage updated to size: " << displayImage.cols << "x" << displayImage.rows
                  << ", channels: " << displayImage.channels() << std::endl;
    }
}

void ImageProcessor::setCurrentImage(const cv::Mat& image) {
    if (!image.empty()) {
        std::cout << "DEBUG: setCurrentImage called with image size: " << image.cols << "x" << image.rows << std::endl;
        currentImage = image.clone();
        updateDisplayImage();
        std::cout << "DEBUG: setCurrentImage completed" << std::endl;
    } else {
        std::cout << "DEBUG: setCurrentImage called with empty image!" << std::endl;
    }
}

void ImageProcessor::applyPreProcessedImage(const cv::Mat& processedImage) {
    if (!processedImage.empty()) {
        std::cout << "DEBUG: applyPreProcessedImage called with image size: " << processedImage.cols << "x" << processedImage.rows
                  << ", channels: " << processedImage.channels() << std::endl;
        std::cout << "DEBUG: currentImage before replacement: channels=" << currentImage.channels() << std::endl;

        // 直接使用处理后的图像，不强制类型转换（支持串行流程中的通道变化）
        currentImage = processedImage.clone();

        std::cout << "DEBUG: currentImage after replacement: channels=" << currentImage.channels() << std::endl;
        std::cout << "DEBUG: Calling updateDisplayImage()" << std::endl;
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

void ImageProcessor::ensureImageLoaded() const {
    if (currentImage.empty()) {
        throw std::runtime_error("No image loaded");
    }
}

cv::Mat ImageProcessor::performKMeans(const cv::Mat& image, int k) {
    cv::Mat data;
    image.reshape(1, image.rows * image.cols).convertTo(data, CV_32F);
    
    cv::Mat labels, centers;
    cv::kmeans(data, k, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 20, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);
    
    cv::Mat result(image.size(), image.type());
    for (int i = 0; i < image.rows * image.cols; i++) {
        int cluster = labels.at<int>(i);
        if (image.channels() == 1) {
            result.at<uchar>(i / image.cols, i % image.cols) = static_cast<uchar>(centers.at<float>(cluster, 0));
        } else {
            cv::Vec3b color(static_cast<uchar>(centers.at<float>(cluster, 0)),
                           static_cast<uchar>(centers.at<float>(cluster, 1)),
                           static_cast<uchar>(centers.at<float>(cluster, 2)));
            result.at<cv::Vec3b>(i / image.cols, i % image.cols) = color;
        }
    }
    
    return result;
}
