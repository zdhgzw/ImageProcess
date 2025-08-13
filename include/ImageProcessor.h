#pragma once

#include <opencv2/opencv.hpp>
#include <string>

/**
 * @brief 图像处理应用程序的核心类
 */
class ImageProcessor {
private:
    cv::Mat originalImage;     // 原始图像
    cv::Mat currentImage;      // 当前处理后的图像
    cv::Mat displayImage;      // 用于显示的图像
    std::string imagePath;     // 图像文件路径

public:
    /**
     * @brief 构造函数
     */
    ImageProcessor();

    /**
     * @brief 析构函数
     */
    ~ImageProcessor();

    /**
     * @brief 加载图像
     * @param imagePath 图像文件路径
     * @return 是否成功加载
     */
    bool loadImage(const std::string& imagePath);

    /**
     * @brief 重置到原始图像
     */
    void resetToOriginal();

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
     * @brief 转换为灰度图像
     */
    void convertToGrayscale();

    /**
     * @brief 颜色选择
     * @param hue_min 色调最小值
     * @param hue_max 色调最大值
     * @param sat_min 饱和度最小值
     * @param sat_max 饱和度最大值
     * @param val_min 明度最小值
     * @param val_max 明度最大值
     */
    void colorSelect(int hue_min, int hue_max, int sat_min, int sat_max, int val_min, int val_max);

    /**
     * @brief K-means颜色聚类
     * @param k 聚类数量
     */
    void colorCluster(int k);

    /**
     * @brief 颜色反褶积
     * @param channel 通道索引 (0=蓝色, 1=绿色, 2=红色)
     */
    void colorDeconvolution(int channel);

    /**
     * @brief 通道操作
     * @param operation 操作类型 ("add", "subtract", "multiply", "divide")
     * @param value 操作值
     */
    void channelOperation(const std::string& operation, double value);

    // 辅助函数
    bool hasImage() const;
    cv::Size getImageSize() const;
    void updateDisplayImage();
    void setCurrentImage(const cv::Mat& image);
    void applyPreProcessedImage(const cv::Mat& processedImage);

    // 公共算法方法
    cv::Mat performKMeans(const cv::Mat& image, int k);
    
    // 预处理功能 - 直接模仿其他工作功能的模式
    void adjustContrast(double brightness, double contrast);
    void applyHistogramEqualization(int method, double clipLimit);
    void flattenBackground(int kernelSize);

private:
    // 内部辅助函数
    void ensureImageLoaded() const;
    
    // 预处理算法实现
    cv::Mat applyAdjustContrast(const cv::Mat& image, double brightness, double contrast);
    cv::Mat applyHistogramEqualization(const cv::Mat& image, int method, double clipLimit);
    cv::Mat applyFlattenBackground(const cv::Mat& image, int kernelSize);
    cv::Mat applyMedianFilter(const cv::Mat& image, int kernelSize);
    cv::Mat applyWienerFilter(const cv::Mat& image, int kernelSize);
    cv::Mat applyNonLocalMeans(const cv::Mat& image, double h, int templateWindowSize, int searchWindowSize);
    cv::Mat applyGaussianBlur(const cv::Mat& image, int kernelSize, double sigmaX, double sigmaY);
    cv::Mat applyAverageBlur(const cv::Mat& image, int kernelSize);
    cv::Mat applySumFilter(const cv::Mat& image, int kernelSize);
    cv::Mat applyGrayscaleDilate(const cv::Mat& image, int kernelSize);
    cv::Mat applyGrayscaleErode(const cv::Mat& image, int kernelSize);
    cv::Mat applyStdDevFilter(const cv::Mat& image, int kernelSize);
    cv::Mat applyEntropyFilter(const cv::Mat& image, int kernelSize);
    cv::Mat applyGradientFilter(const cv::Mat& image);
    cv::Mat applyHighlightLines(const cv::Mat& image);
    cv::Mat applyBrightTexture(const cv::Mat& image, int kernelSize);
    cv::Mat applyDarkTexture(const cv::Mat& image, int kernelSize);
    cv::Mat applyAdvancedTexture(const cv::Mat& image, int kernelSize);
    cv::Mat applySimilarity(const cv::Mat& image, int kernelSize);
    cv::Mat applySharpen(const cv::Mat& image, double strength);
    cv::Mat applyFFTFilter(const cv::Mat& image);
    cv::Mat applyGrayscaleInterpolation(const cv::Mat& image);
    cv::Mat applyGrayscaleReconstruction(const cv::Mat& image);
};
