#pragma once

#include <opencv2/opencv.hpp>

/**
 * @brief 形态学功能枚举
 */
enum class MorphologyFunction {
    NONE = -1,
    // MORPHOLOGY (形态学操作)
    DILATE = 0,
    ERODE = 1,
    OPENING = 2,
    CLOSING = 3,
    GRADIENT = 4,
    TOP_HAT = 5,
    BLACK_HAT = 6,
    // UNIFORM (统一操作)
    DILATE_UNIFORM = 7,
    ERODE_UNIFORM = 8,
    // RETAIN (保留操作)
    DILATE_RETAIN = 9,
    ERODE_RETAIN = 10,
    // EDGES (边界调节)
    SEPARATE_FEATURES = 11
};

/**
 * @brief 形态学算法类
 * 包含所有形态学功能的实现
 */
class Morphology {
public:
    /**
     * @brief 构造函数
     */
    Morphology();

    /**
     * @brief 析构函数
     */
    ~Morphology();

    // 形态学操作算法
    static cv::Mat dilate(const cv::Mat& image, int kernelSize, int kernelType);
    static cv::Mat erode(const cv::Mat& image, int kernelSize, int kernelType);
    
    // Uniform操作算法（像素值精确控制）
    static cv::Mat dilateUniform(const cv::Mat& image, int pixelValue);
    static cv::Mat erodeUniform(const cv::Mat& image, int pixelValue);
    static cv::Mat opening(const cv::Mat& image, int kernelSize, int kernelType);
    static cv::Mat closing(const cv::Mat& image, int kernelSize, int kernelType);
    static cv::Mat gradient(const cv::Mat& image, int kernelSize, int kernelType);
    static cv::Mat topHat(const cv::Mat& image, int kernelSize, int kernelType);
    static cv::Mat blackHat(const cv::Mat& image, int kernelSize, int kernelType);
    
    // Retain操作算法
    static cv::Mat dilateRetain(const cv::Mat& image, int kernelSize, int kernelType, int retainMethod);
    static cv::Mat erodeRetain(const cv::Mat& image, int kernelSize, int kernelType, int retainMethod);

    /**
     * @brief 应用形态学功能
     * @param image 输入图像
     * @param function 形态学功能
     * @param params 参数数组
     * @return 处理后的图像
     */
    static cv::Mat applyFunction(const cv::Mat& image, MorphologyFunction function, const std::vector<double>& params);

    /**
     * @brief 获取形态学核
     * @param kernelType 核类型 (0=RECT, 1=ELLIPSE, 2=CROSS)
     * @param kernelSize 核大小
     * @return 形态学核
     */
    static cv::Mat getKernel(int kernelType, int kernelSize);
};
