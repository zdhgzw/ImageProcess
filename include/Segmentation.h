#pragma once

#include <opencv2/opencv.hpp>

/**
 * @brief 分割功能枚举
 */
enum class SegmentationFunction {
    NONE = -1,
    // THRESHOLD (阈值标记)
    BASIC_THRESHOLD = 0,
    RANGE_THRESHOLD = 1,
    ADAPTIVE_THRESHOLD = 2,
    EM_THRESHOLD = 3,
    LOCAL_THRESHOLD = 4
    //EDGES (边界识别)
    //todo 分水岭分割
};

/**
 * @brief 图像分割算法类
 * 包含所有分割功能的实现
 */
class Segmentation {
public:
    /**
     * @brief 构造函数
     */
    Segmentation();

    /**
     * @brief 析构函数
     */
    ~Segmentation();

    // THRESHOLD类别算法
    static cv::Mat basicThreshold(const cv::Mat& image, double threshold, int type);
    static cv::Mat rangeThreshold(const cv::Mat& image, double minVal, double maxVal);
    static cv::Mat adaptiveThreshold(const cv::Mat& image, int method, int type, int blockSize, double C);
    static cv::Mat emThreshold(const cv::Mat& image);
    static cv::Mat localThreshold(const cv::Mat& image, int blockSize = 11, double C = 2.0);

    /**
     * @brief 应用分割功能
     * @param image 输入图像
     * @param function 分割功能
     * @param params 参数数组
     * @return 处理后的图像
     */
    static cv::Mat applyFunction(const cv::Mat& image, SegmentationFunction function, const std::vector<double>& params);
};
