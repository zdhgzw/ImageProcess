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
    OTSU_THRESHOLD = 3,
    LOCAL_THRESHOLD = 4,
    //EDGES (边界识别)
    WATERSHED = 5    // 分水岭分割
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
    static cv::Mat otsuThreshold(const cv::Mat& image);
    static cv::Mat localThreshold(const cv::Mat& image, int blockSize = 11, double C = 2.0);
    
    /**
     * @brief 分水岭分割
     * @param image 输入图像（预处理后的二值图像）
     * @param originalImage 原始彩色图像（用于分水岭算法和结果显示）
     * @param distanceThreshold 距离变换阈值（0.1-0.9，控制前景区域大小）
     * @param showVisualization 是否显示可视化画布（默认false，仅在预览时显示）
     * @return 在原始图像基础上标记分割结果的彩色图像
     */
    static cv::Mat watershed(const cv::Mat& image, const cv::Mat& originalImage, double distanceThreshold = 0.5, bool showVisualization = false);

    /**
     * @brief 应用分割功能
     * @param image 输入图像
     * @param function 分割功能
     * @param params 参数数组
     * @return 处理后的图像
     */
    static cv::Mat applyFunction(const cv::Mat& image, SegmentationFunction function, const std::vector<double>& params);
};
