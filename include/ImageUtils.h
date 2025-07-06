#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <opencv2/opencv.hpp>
#include <string>

/**
 * 图像处理工具类
 * 提供图像读取、保存和基本操作的通用功能
 */
class ImageUtils {
public:
    /**
     * 读取图像文件
     * @param imagePath 图像文件路径
     * @return 读取的图像，如果失败返回空的Mat对象
     */
    static cv::Mat loadImage(const std::string& imagePath);

    /**
     * 保存图像文件
     * @param image 要保存的图像
     * @param outputPath 输出文件路径
     * @return 保存是否成功
     */
    static bool saveImage(const cv::Mat& image, const std::string& outputPath);

    /**
     * 检查图像是否有效
     * @param image 要检查的图像
     * @return 图像是否有效
     */
    static bool isValidImage(const cv::Mat& image);

    /**
     * 获取图像信息字符串
     * @param image 图像对象
     * @return 包含图像尺寸、通道数等信息的字符串
     */
    static std::string getImageInfo(const cv::Mat& image);

    /**
     * 创建输出目录（如果不存在）
     * @param outputPath 输出文件路径
     * @return 目录创建是否成功
     */
    static bool createOutputDirectory(const std::string& outputPath);

private:
    ImageUtils() = default;
};

#endif // IMAGE_UTILS_H
