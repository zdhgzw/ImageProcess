#pragma once

#include <opencv2/opencv.hpp>
#include <string>

/**
 * @brief 测量功能枚举
 */
enum class MeasurementsFunction {
    NONE = -1,
    // PER IMAGE (每图像)
    COUNT = 0
};

/**
 * @brief 测量结果结构体
 */
struct MeasurementResult {
    int objectCount;
    double averageSize;
    double totalArea;
    std::string details;
    
    MeasurementResult() : objectCount(0), averageSize(0.0), totalArea(0.0), details("") {}
};

/**
 * @brief 图像测量算法类
 * 包含所有测量功能的实现
 */
class Measurements {
public:
    /**
     * @brief 构造函数
     */
    Measurements();

    /**
     * @brief 析构函数
     */
    ~Measurements();

    // PER IMAGE类别算法
    static MeasurementResult countObjects(const cv::Mat& image, int minSize, int maxSize, double sensitivity);

    /**
     * @brief 应用测量功能
     * @param image 输入图像
     * @param function 测量功能
     * @param params 参数数组
     * @return 测量结果
     */
    static MeasurementResult applyFunction(const cv::Mat& image, MeasurementsFunction function, const std::vector<double>& params);

    /**
     * @brief 创建带有计数结果的可视化图像
     * @param image 输入图像
     * @param result 测量结果
     * @param minSize 最小尺寸阈值
     * @param maxSize 最大尺寸阈值
     * @return 带有标注的图像
     */
    static cv::Mat createVisualizationImage(const cv::Mat& image, const MeasurementResult& result, int minSize, int maxSize);

private:
    /**
     * @brief 检测并计数对象
     * @param image 输入图像
     * @param minSize 最小对象大小
     * @param maxSize 最大对象大小
     * @param sensitivity 检测敏感度
     * @return 检测到的轮廓
     */
    static std::vector<std::vector<cv::Point>> detectObjects(const cv::Mat& image, int minSize, int maxSize, double sensitivity);
    
    /**
     * @brief 计算对象统计信息
     * @param contours 对象轮廓
     * @return 测量结果
     */
    static MeasurementResult calculateStatistics(const std::vector<std::vector<cv::Point>>& contours);
};
