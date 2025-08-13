#include "PreProcessing.h"
#include <iostream>

PreProcessing::PreProcessing() {
}

PreProcessing::~PreProcessing() {
}

// CONTRAST类别算法实现
cv::Mat PreProcessing::adjustContrast(const cv::Mat& image, double brightness, double contrast) {
    cv::Mat result;
    image.convertTo(result, image.type(), contrast, brightness);
    std::cout << "DEBUG: PreProcessing::adjustContrast - input: " << image.cols << "x" << image.rows 
              << ", output: " << result.cols << "x" << result.rows << std::endl;
    return result;
}

cv::Mat PreProcessing::histogramEqualization(const cv::Mat& image, int method, double clipLimit) {
    cv::Mat result;
    
    if (method == 0) {
        // Global histogram equalization
        if (image.channels() == 1) {
            cv::equalizeHist(image, result);
        } else {
            cv::Mat ycrcb;
            cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
            std::vector<cv::Mat> channels;
            cv::split(ycrcb, channels);
            cv::equalizeHist(channels[0], channels[0]);
            cv::merge(channels, ycrcb);
            cv::cvtColor(ycrcb, result, cv::COLOR_YCrCb2BGR);
        }
    } else {
        // Adaptive histogram equalization (CLAHE)
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clipLimit, cv::Size(8, 8));
        
        if (image.channels() == 1) {
            clahe->apply(image, result);
        } else {
            cv::Mat ycrcb;
            cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
            std::vector<cv::Mat> channels;
            cv::split(ycrcb, channels);
            clahe->apply(channels[0], channels[0]);
            cv::merge(channels, ycrcb);
            cv::cvtColor(ycrcb, result, cv::COLOR_YCrCb2BGR);
        }
    }
    return result;
}

cv::Mat PreProcessing::flattenBackground(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(image, result, cv::MORPH_TOPHAT, kernel);
    return result;
}

// NOISE-REDUCTION类别算法实现
cv::Mat PreProcessing::medianFilter(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::medianBlur(image, result, kernelSize);
    return result;
}

cv::Mat PreProcessing::wienerFilter(const cv::Mat& image, int kernelSize) {
    // 简化的Wiener滤波实现 - 使用双边滤波近似
    cv::Mat result;
    cv::bilateralFilter(image, result, kernelSize, kernelSize * 2, kernelSize / 2);
    return result;
}

cv::Mat PreProcessing::nonLocalMeans(const cv::Mat& image, double h, int templateWindowSize, int searchWindowSize) {
    cv::Mat result;
    if (image.channels() == 1) {
        cv::fastNlMeansDenoising(image, result, h, templateWindowSize, searchWindowSize);
    } else {
        cv::fastNlMeansDenoisingColored(image, result, h, h, templateWindowSize, searchWindowSize);
    }
    return result;
}

// BLUR类别算法实现
cv::Mat PreProcessing::gaussianBlur(const cv::Mat& image, int kernelSize, double sigmaX, double sigmaY) {
    cv::Mat result;
    cv::GaussianBlur(image, result, cv::Size(kernelSize, kernelSize), sigmaX, sigmaY);
    return result;
}

cv::Mat PreProcessing::averageBlur(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::blur(image, result, cv::Size(kernelSize, kernelSize));
    return result;
}

cv::Mat PreProcessing::sumFilter(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::Mat::ones(kernelSize, kernelSize, CV_32F);
    cv::filter2D(image, result, -1, kernel);
    return result;
}

cv::Mat PreProcessing::grayscaleDilate(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::dilate(image, result, kernel);
    return result;
}

cv::Mat PreProcessing::grayscaleErode(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::erode(image, result, kernel);
    return result;
}

// EDGES类别算法实现
cv::Mat PreProcessing::stdDevFilter(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat mean, sqrMean;
    cv::blur(image, mean, cv::Size(kernelSize, kernelSize));
    cv::Mat squared;
    cv::multiply(image, image, squared);
    cv::blur(squared, sqrMean, cv::Size(kernelSize, kernelSize));
    cv::Mat meanSquared;
    cv::multiply(mean, mean, meanSquared);
    cv::sqrt(sqrMean - meanSquared, result);
    return result;
}

cv::Mat PreProcessing::entropyFilter(const cv::Mat& image, int kernelSize) {
    // 简化的熵滤波实现 - 使用局部标准差近似
    return stdDevFilter(image, kernelSize);
}

cv::Mat PreProcessing::gradientFilter(const cv::Mat& image) {
    cv::Mat result;
    cv::Mat grad_x, grad_y;
    cv::Sobel(image, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(image, grad_y, CV_16S, 0, 1, 3);
    cv::convertScaleAbs(grad_x, grad_x);
    cv::convertScaleAbs(grad_y, grad_y);
    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, result);
    return result;
}

cv::Mat PreProcessing::highlightLines(const cv::Mat& image) {
    cv::Mat result;
    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150);

    if (image.channels() == 3) {
        cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
    } else {
        result = edges;
    }
    return result;
}

// TEXTURE类别算法实现
cv::Mat PreProcessing::brightTexture(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::Mat::ones(kernelSize, kernelSize, CV_32F) / (float)(kernelSize * kernelSize);
    cv::filter2D(image, result, -1, kernel);
    cv::subtract(image, result, result);
    cv::add(result, cv::Scalar::all(128), result);
    return result;
}

cv::Mat PreProcessing::darkTexture(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::Mat::ones(kernelSize, kernelSize, CV_32F) / (float)(kernelSize * kernelSize);
    cv::filter2D(image, result, -1, kernel);
    cv::subtract(result, image, result);
    cv::add(result, cv::Scalar::all(128), result);
    return result;
}

cv::Mat PreProcessing::advancedTexture(const cv::Mat& image, int kernelSize) {
    // 高级纹理增强 - 使用拉普拉斯算子
    cv::Mat result;
    cv::Mat laplacian;
    cv::Laplacian(image, laplacian, CV_16S, kernelSize);
    cv::convertScaleAbs(laplacian, result);
    return result;
}

cv::Mat PreProcessing::similarity(const cv::Mat& image, int kernelSize) {
    // 相似性滤波 - 使用双边滤波
    cv::Mat result;
    cv::bilateralFilter(image, result, kernelSize, kernelSize * 2, kernelSize / 2);
    return result;
}

// CORRECTION类别算法实现
cv::Mat PreProcessing::sharpen(const cv::Mat& image, double strength) {
    cv::Mat result;
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
                      0, -1 * strength, 0,
                      -1 * strength, 1 + 4 * strength, -1 * strength,
                      0, -1 * strength, 0);
    cv::filter2D(image, result, -1, kernel);
    return result;
}

cv::Mat PreProcessing::fftFilter(const cv::Mat& image) {
    // 简化的FFT滤波 - 使用高斯低通滤波近似
    cv::Mat result;
    cv::GaussianBlur(image, result, cv::Size(15, 15), 2.0, 2.0);
    return result;
}

cv::Mat PreProcessing::grayscaleInterpolation(const cv::Mat& image) {
    // 灰度插值 - 使用双线性插值放大后缩小
    cv::Mat result, temp;
    cv::resize(image, temp, cv::Size(image.cols * 2, image.rows * 2), 0, 0, cv::INTER_LINEAR);
    cv::resize(temp, result, image.size(), 0, 0, cv::INTER_LINEAR);
    return result;
}

cv::Mat PreProcessing::grayscaleReconstruction(const cv::Mat& image) {
    // 灰度重建 - 使用形态学重建的简化版本
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(image, result, cv::MORPH_CLOSE, kernel);
    return result;
}

// 统一的应用函数
cv::Mat PreProcessing::applyFunction(const cv::Mat& image, PreProcessingFunction function, const std::vector<double>& params) {
    switch (function) {
        case PreProcessingFunction::ADJUST_CONTRAST:
            return adjustContrast(image, params.size() > 0 ? params[0] : 0.0, params.size() > 1 ? params[1] : 1.0);
        case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
            return histogramEqualization(image, params.size() > 0 ? (int)params[0] : 0, params.size() > 1 ? params[1] : 2.0);
        case PreProcessingFunction::FLATTEN_BACKGROUND:
            return flattenBackground(image, params.size() > 0 ? (int)params[0] : 15);
        case PreProcessingFunction::MEDIAN_FILTER:
            return medianFilter(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::WIENER_FILTER:
            return wienerFilter(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::NON_LOCAL_MEANS:
            return nonLocalMeans(image, params.size() > 0 ? params[0] : 10.0,
                               params.size() > 1 ? (int)params[1] : 7, params.size() > 2 ? (int)params[2] : 21);
        case PreProcessingFunction::GAUSSIAN_BLUR:
            return gaussianBlur(image, params.size() > 0 ? (int)params[0] : 5,
                              params.size() > 1 ? params[1] : 1.0, params.size() > 2 ? params[2] : 1.0);
        case PreProcessingFunction::AVERAGE_BLUR:
            return averageBlur(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::SUM_FILTER:
            return sumFilter(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::GRAYSCALE_DILATE:
            return grayscaleDilate(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::GRAYSCALE_ERODE:
            return grayscaleErode(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::STDDEV_FILTER:
            return stdDevFilter(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::ENTROPY_FILTER:
            return entropyFilter(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::GRADIENT_FILTER:
            return gradientFilter(image);
        case PreProcessingFunction::HIGHLIGHT_LINES:
            return highlightLines(image);
        case PreProcessingFunction::BRIGHT_TEXTURE:
            return brightTexture(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::DARK_TEXTURE:
            return darkTexture(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::ADVANCED_TEXTURE:
            return advancedTexture(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::SIMILARITY:
            return similarity(image, params.size() > 0 ? (int)params[0] : 5);
        case PreProcessingFunction::SHARPEN:
            return sharpen(image, params.size() > 0 ? params[0] : 1.0);
        case PreProcessingFunction::FFT_FILTER:
            return fftFilter(image);
        case PreProcessingFunction::GRAYSCALE_INTERPOLATION:
            return grayscaleInterpolation(image);
        case PreProcessingFunction::GRAYSCALE_RECONSTRUCTION:
            return grayscaleReconstruction(image);
        default:
            return image.clone();
    }
}
