#pragma once

#include "ImageProcessor.h"
#include <opencv2/opencv.hpp>
#include <string>

/**
 * @brief 图像处理应用程序的主界面类
 */
class ImageProcessingApp {
private:
    ImageProcessor processor;
    cv::Mat frame;
    std::string windowName;
    
    // UI状态变量
    int windowWidth;
    int windowHeight;
    int imageDisplayX;
    int imageDisplayY;
    int imageDisplayWidth;
    int imageDisplayHeight;
    int controlPanelX;
    int controlPanelY;
    int controlPanelWidth;
    
    // 颜色选择参数
    int hue_min, hue_max;
    int sat_min, sat_max;
    int val_min, val_max;
    
    // K-means聚类参数
    int k_clusters;
    
    // 通道选择
    int selected_channel;
    
    // 通道操作参数
    int operation_type;  // 0=add, 1=subtract, 2=multiply, 3=divide
    double operation_value;
    
public:
    ImageProcessingApp();
    ~ImageProcessingApp();
    
    /**
     * @brief 初始化应用程序
     */
    void initialize();
    
    /**
     * @brief 运行主循环
     */
    void run();
    
private:
    /**
     * @brief 初始化UI参数
     */
    void initializeUI();
    
    /**
     * @brief 渲染主界面
     */
    void renderUI();
    
    /**
     * @brief 渲染图像加载面板
     */
    void renderImageLoadPanel();
    
    /**
     * @brief 渲染彩色图像处理面板
     */
    void renderColorProcessingPanel();
    
    /**
     * @brief 渲染图像显示区域
     */
    void renderImageDisplay();
    
    /**
     * @brief 打开文件对话框
     * @return 选择的文件路径
     */
    std::string openFileDialog();
    
    /**
     * @brief 缩放图像以适应显示区域
     * @param image 原始图像
     * @return 缩放后的图像
     */
    cv::Mat scaleImageToFit(const cv::Mat& image);
};