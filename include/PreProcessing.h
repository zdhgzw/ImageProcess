#pragma once

#include <opencv2/opencv.hpp>

/**
 * @brief 预处理功能枚举
 */
enum class PreProcessingFunction {
    NONE = -1,
    // CONTRAST (对比度调节)
    ADJUST_CONTRAST = 0,
    HISTOGRAM_EQUALIZATION = 1,
    FLATTEN_BACKGROUND = 2,
    // NOISE-REDUCTION (降噪处理)
    MEDIAN_FILTER = 3,
    WIENER_FILTER = 4,
    NON_LOCAL_MEANS = 5,
    // BLUR (模糊处理)
    GAUSSIAN_BLUR = 6,
    AVERAGE_BLUR = 7,
    SUM_FILTER = 8,
    GRAYSCALE_DILATE = 9,
    GRAYSCALE_ERODE = 10,
    // EDGES (突出边界)
    STDDEV_FILTER = 11,
    ENTROPY_FILTER = 12,
    GRADIENT_FILTER = 13,
    HIGHLIGHT_LINES = 14,
    // TEXTURE (突出纹理特征)
    BRIGHT_TEXTURE = 15,
    DARK_TEXTURE = 16,
    ADVANCED_TEXTURE = 17,
    SIMILARITY = 18,
    // CORRECTION (图像修正)
    SHARPEN = 19,
    FFT_FILTER = 20,
    GRAYSCALE_INTERPOLATION = 21,
    GRAYSCALE_RECONSTRUCTION = 22
};

/**
 * @brief 预处理算法类
 * 包含所有预处理功能的实现
 */
class PreProcessing {
public:
    /**
     * @brief 构造函数
     */
    PreProcessing();

    /**
     * @brief 析构函数
     */
    ~PreProcessing();

    // CONTRAST类别算法
    static cv::Mat adjustContrast(const cv::Mat& image, double brightness, double contrast);
    static cv::Mat histogramEqualization(const cv::Mat& image, int method, double clipLimit);
    static cv::Mat flattenBackground(const cv::Mat& image, int kernelSize);

    // NOISE-REDUCTION类别算法
    //kernelSize 1~31, must be odd
    static cv::Mat medianFilter(const cv::Mat& image, int kernelSize);
    static cv::Mat wienerFilter(const cv::Mat& image, int kernelSize);
    static cv::Mat nonLocalMeans(const cv::Mat& image, double h, int templateWindowSize, int searchWindowSize);

    // BLUR类别算法
    static cv::Mat gaussianBlur(const cv::Mat& image, int kernelSize, double sigmaX, double sigmaY);
    static cv::Mat averageBlur(const cv::Mat& image, int kernelSize);
    static cv::Mat sumFilter(const cv::Mat& image, int kernelSize);
    static cv::Mat grayscaleDilate(const cv::Mat& image, int kernelSize);
    static cv::Mat grayscaleErode(const cv::Mat& image, int kernelSize);

    // EDGES类别算法
    static cv::Mat stdDevFilter(const cv::Mat& image, int kernelSize);
    static cv::Mat entropyFilter(const cv::Mat& image, int kernelSize);
    static cv::Mat gradientFilter(const cv::Mat& image);
    static cv::Mat highlightLines(const cv::Mat& image);

    // TEXTURE类别算法
    static cv::Mat brightTexture(const cv::Mat& image, int kernelSize);
    static cv::Mat darkTexture(const cv::Mat& image, int kernelSize);
    static cv::Mat advancedTexture(const cv::Mat& image, int kernelSize);
    static cv::Mat similarity(const cv::Mat& image, int kernelSize);

    // CORRECTION类别算法
    static cv::Mat sharpen(const cv::Mat& image, double strength);
    static cv::Mat fftFilter(const cv::Mat& image);
    static cv::Mat grayscaleInterpolation(const cv::Mat& image);
    static cv::Mat grayscaleReconstruction(const cv::Mat& image);

    /**
     * @brief 应用预处理功能
     * @param image 输入图像
     * @param function 预处理功能
     * @param params 参数数组
     * @return 处理后的图像
     */
    static cv::Mat applyFunction(const cv::Mat& image, PreProcessingFunction function, const std::vector<double>& params);
};
