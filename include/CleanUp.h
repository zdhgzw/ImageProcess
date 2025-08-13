#pragma once

#include <opencv2/opencv.hpp>

/**
 * @brief 清理功能枚举
 */
enum class CleanUpFunction {
    NONE = -1,
    // REJECT (拒绝/清理)
    FILL_ALL_HOLES = 0,
    REJECT_FEATURES = 1
};

/**
 * @brief 图像清理算法类
 * 包含所有清理功能的实现
 */
class CleanUp {
public:
    /**
     * @brief 构造函数
     */
    CleanUp();

    /**
     * @brief 析构函数
     */
    ~CleanUp();

    // REJECT类别算法
    static cv::Mat fillAllHoles(const cv::Mat& image, int minHoleSize, int fillMethod);
    static cv::Mat rejectFeatures(const cv::Mat& image, int minFeatureSize, int maxFeatureSize, int rejectMethod);

    /**
     * @brief 应用清理功能
     * @param image 输入图像
     * @param function 清理功能
     * @param params 参数数组
     * @return 处理后的图像
     */
    static cv::Mat applyFunction(const cv::Mat& image, CleanUpFunction function, const std::vector<double>& params);

private:
    /**
     * @brief 查找并填充孔洞
     * @param image 输入图像
     * @param minSize 最小孔洞大小
     * @return 填充后的图像
     */
    static cv::Mat findAndFillHoles(const cv::Mat& image, int minSize);
    
    /**
     * @brief 基于大小拒绝特征
     * @param image 输入图像
     * @param minSize 最小特征大小
     * @param maxSize 最大特征大小
     * @return 处理后的图像
     */
    static cv::Mat rejectFeaturesBySize(const cv::Mat& image, int minSize, int maxSize);
};
