#pragma once

#include <opencv2/opencv.hpp>
#include "PreProcessing.h"
#include "Segmentation.h"
#include "Morphology.h"
#include "CleanUp.h"
#include "Measurements.h"

/**
 * @brief UI组件和辅助函数类
 * 包含模态窗口渲染和UI辅助功能
 */
class UIComponents {
public:
    /**
     * @brief 构造函数
     */
    UIComponents();

    /**
     * @brief 析构函数
     */
    ~UIComponents();

    /**
     * @brief 缩放图像以适应显示区域
     * @param image 输入图像
     * @param maxWidth 最大宽度
     * @param maxHeight 最大高度
     * @return 缩放后的图像
     */
    static cv::Mat scaleImageToFit(const cv::Mat& image, int maxWidth = 800, int maxHeight = 400);

    /**
     * @brief 渲染预览区域
     * @param frame 主窗口frame
     * @param x X坐标
     * @param y Y坐标
     * @param width 宽度
     * @param height 高度
     * @param previewImage 预览图像
     */
    static void renderPreviewArea(cv::Mat& frame, int x, int y, int width, int height, const cv::Mat& previewImage);

    /**
     * @brief 渲染模态窗口按钮
     * @param frame 主窗口frame
     * @param x X坐标
     * @param y Y坐标
     * @return 按钮点击结果 (0=无, 1=Apply, 2=Cancel)
     */
    static int renderModalButtons(cv::Mat& frame, int x, int y);

    /**
     * @brief 渲染Adjust Contrast参数控制
     * @param frame 主窗口frame
     * @param startY 起始Y坐标
     * @param controlAreaX 控制区域X坐标
     * @param brightness 亮度参数引用
     * @param contrast 对比度参数引用
     * @param prevBrightness 前一个亮度值引用
     * @param prevContrast 前一个对比度值引用
     * @param currentFunction 当前功能
     * @return 是否需要更新预览
     */
    static bool renderAdjustContrastParameters(cv::Mat& frame, int startY, int controlAreaX, 
                                             double& brightness, double& contrast,
                                             double& prevBrightness, double& prevContrast,
                                             PreProcessingFunction currentFunction);

    /**
     * @brief 渲染Histogram Equalization参数控制
     * @param frame 主窗口frame
     * @param startY 起始Y坐标
     * @param controlAreaX 控制区域X坐标
     * @param histogramMethod 直方图方法引用
     * @param clipLimit 剪切限制引用
     * @param prevClipLimit 前一个剪切限制值引用
     * @param currentFunction 当前功能
     * @return 是否需要更新预览
     */
    static bool renderHistogramEqualizationParameters(cv::Mat& frame, int startY, int controlAreaX,
                                                     int& histogramMethod, double& clipLimit,
                                                     double& prevClipLimit, PreProcessingFunction currentFunction);

    /**
     * @brief 渲染Flatten Background参数控制
     * @param frame 主窗口frame
     * @param startY 起始Y坐标
     * @param controlAreaX 控制区域X坐标
     * @param flattenKernelSize 核大小引用
     * @param prevFlattenKernelSize 前一个核大小值引用
     * @param currentFunction 当前功能
     * @return 是否需要更新预览
     */
    static bool renderFlattenBackgroundParameters(cv::Mat& frame, int startY, int controlAreaX,
                                                 int& flattenKernelSize, int& prevFlattenKernelSize,
                                                 PreProcessingFunction currentFunction);

    /**
     * @brief 渲染预处理功能选择界面
     * @param frame 主窗口frame
     * @param controlAreaX 控制区域X坐标
     * @param controlAreaY 控制区域Y坐标
     * @return 选择的预处理功能
     */
    static PreProcessingFunction renderPreProcessingFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY);

    /**
     * @brief 渲染预处理参数控制界面
     * @param frame 主窗口frame
     * @param controlAreaX 控制区域X坐标
     * @param controlAreaY 控制区域Y坐标
     * @param currentFunction 当前功能
     * @param brightness 亮度参数引用
     * @param contrast 对比度参数引用
     * @param histogramMethod 直方图方法引用
     * @param clipLimit 剪切限制引用
     * @param flattenKernelSize 核大小引用
     * @param prevBrightness 前一个亮度值引用
     * @param prevContrast 前一个对比度值引用
     * @param prevClipLimit 前一个剪切限制值引用
     * @param prevFlattenKernelSize 前一个核大小值引用
     * @return 操作结果 (0=无, 1=返回, 2=更新预览)
     */
    static int renderPreProcessingParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                           PreProcessingFunction currentFunction,
                                           double& brightness, double& contrast,
                                           int& histogramMethod, double& clipLimit, int& flattenKernelSize,
                                           double& prevBrightness, double& prevContrast,
                                           double& prevClipLimit, int& prevFlattenKernelSize);

    // Segmentation UI methods
    static SegmentationFunction renderSegmentationFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY);
    static int renderSegmentationParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                          SegmentationFunction currentFunction,
                                          double& thresholdValue, int& thresholdType,
                                          double& thresholdMin, double& thresholdMax,
                                          int& adaptiveMethod, int& blockSize, double& C);

    // Morphology UI methods
    static MorphologyFunction renderMorphologyFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY);
    static int renderMorphologyParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                        MorphologyFunction currentFunction,
                                        int& morphKernelSize, int& morphKernelType,
                                        double& edgeThreshold, int& separationMethod);

    // CleanUp UI methods
    static CleanUpFunction renderCleanUpFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY);
    static int renderCleanUpParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                     CleanUpFunction currentFunction,
                                     int& minHoleSize, int& fillMethod,
                                     int& minFeatureSize, int& maxFeatureSize, int& rejectMethod);

    // Measurements UI methods
    static MeasurementsFunction renderMeasurementsFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY);
    static int renderMeasurementsParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                          MeasurementsFunction currentFunction,
                                          int& minObjectSize, int& maxObjectSize, double& sensitivity);
    static void renderMeasurementResults(cv::Mat& frame, int x, int y, int width, int height, const MeasurementResult& result);
};
