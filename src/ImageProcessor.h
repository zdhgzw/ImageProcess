#pragma once

#include <opencv2/opencv.hpp>
#include <string>

/**
 * @brief 图像处理应用程序的核心类
 */
class ImageProcessor {
private:
    cv::Mat originalImage;      // 原始图像
    cv::Mat currentImage;       // 当前处理的图像
    cv::Mat displayImage;       // 用于显示的图像
    std::string imagePath;      // 图像路径
    
public:
    ImageProcessor();
    ~ImageProcessor();
    
    // === 1. Image (图片加载与显示) ===
    
    /**
     * @brief 加载图片
     * @param path 图片路径
     * @return 是否成功加载
     */
    bool loadImage(const std::string& path);
    
    /**
     * @brief 获取当前图像
     * @return 当前图像的引用
     */
    const cv::Mat& getCurrentImage() const;
    
    /**
     * @brief 获取显示图像
     * @return 显示图像的引用
     */
    const cv::Mat& getDisplayImage() const;
    
    /**
     * @brief 重置到原始图像
     */
    void resetToOriginal();
    
    // === 2. Color (彩色图像处理) ===
    
    /**
     * @brief 转换为灰度图像
     */
    void convertToGrayscale();
    
    /**
     * @brief 颜色选择 - 根据HSV范围选择像素
     * @param hue_min 色调最小值 (0-180)
     * @param hue_max 色调最大值 (0-180)
     * @param sat_min 饱和度最小值 (0-255)
     * @param sat_max 饱和度最大值 (0-255)
     * @param val_min 明度最小值 (0-255)
     * @param val_max 明度最大值 (0-255)
     */
    void colorSelect(int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max);
    
    /**
     * @brief 颜色聚类 - 使用K-Means算法
     * @param k 聚类数量
     */
    void colorCluster(int k);
    
    /**
     * @brief 颜色反褶积 - 分离特定颜色通道
     * @param channel 通道索引 (0=B, 1=G, 2=R)
     */
    void colorDeconvolution(int channel);
    
    /**
     * @brief 通道操作 - 对多通道图像执行算术运算
     * @param operation 操作类型: "add", "subtract", "multiply", "divide"
     * @param value 操作值
     */
    void channelOperation(const std::string& operation, double value);
    
    // 辅助函数
    bool hasImage() const;
    cv::Size getImageSize() const;
    void updateDisplayImage();
    void setCurrentImage(const cv::Mat& image);
    void applyPreProcessedImage(const cv::Mat& processedImage);

    // 预处理功能 - 直接模仿其他工作功能的模式
    void adjustContrast(double brightness, double contrast);
    void applyHistogramEqualization(int method, double clipLimit);
    void flattenBackground(int kernelSize);

    // 分割功能 - 阈值标记
    void basicThreshold(double threshold, int type);
    void rangeThreshold(double minVal, double maxVal);
    void adaptiveThreshold(int method, int type, int blockSize, double C);
    void emThreshold();
    void localThreshold();
    
private:
    // 内部辅助函数
    cv::Mat performKMeans(const cv::Mat& image, int k);
    void ensureImageLoaded() const;
};