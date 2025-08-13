#pragma once

#include "ImageProcessor.h"
#include "PreProcessing.h"
#include "Segmentation.h"
#include "Morphology.h"
#include "CleanUp.h"
#include "Measurements.h"
#include "UIComponents.h"
#include <opencv2/opencv.hpp>
#include <string>
#include "cvui.h"

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

    // 颜色反褶积参数
    int deconvolution_channel;

    // 通道操作参数
    int operation_type;
    double operation_value;

    // 通道选择
    int selected_channel;

    // UI控制变量
    std::string selectedImagePath;

    // 统一模态窗口系统
    enum class ModalFunction {
        NONE = -1,
        LOAD_IMAGE = 0,
        CONVERT_GRAYSCALE = 1,
        COLOR_SELECT = 2,
        COLOR_CLUSTER = 3,
        COLOR_DECONVOLUTION = 4,
        CHANNEL_OPERATION = 5,
        RESET_IMAGE = 6,
        PRE_PROCESSING = 7,
        SEGMENTATION = 8,
        MORPHOLOGY = 9,
        CLEAN_UP = 10,
        MEASUREMENTS = 11
    };





    ModalFunction currentModal;     // 当前显示的模态窗口
    cv::Mat previewImage;          // 预览图像
    bool isProcessing;             // 是否正在处理中

    // 模态窗口布局
    int modalWindowX, modalWindowY;
    int modalWindowWidth, modalWindowHeight;
    int previewAreaWidth, previewAreaHeight;
    int controlAreaX, controlAreaY;

    // 预处理功能相关
    PreProcessingFunction currentPreProcessingFunction;

    // 分割功能相关
    SegmentationFunction currentSegmentationFunction;

    // 形态学功能相关
    MorphologyFunction currentMorphologyFunction;

    // 清理功能相关
    CleanUpFunction currentCleanUpFunction;

    // 测量功能相关
    MeasurementsFunction currentMeasurementsFunction;
    MeasurementResult lastMeasurementResult;

    // 预处理参数
    double brightness;             // 亮度调整 (-100 to +100)
    double contrast;              // 对比度调整 (0.1 to 3.0)
    int kernelSize;               // 核大小 (用于各种滤波器)
    double sigmaX, sigmaY;        // 高斯模糊参数
    double h;                     // 非局部均值去噪参数
    int templateWindowSize;       // 模板窗口大小
    int searchWindowSize;         // 搜索窗口大小
    double sharpenStrength;       // 锐化强度

    // CONTRAST类别特定参数
    int histogramMethod;          // 直方图均衡化方法 (0=global, 1=adaptive)
    double clipLimit;             // CLAHE剪切限制 (1.0 to 40.0)
    int flattenKernelSize;        // 背景平坦化核大小 (5 to 51, odd only)

    // 阈值标记参数
    double thresholdValue;        // 基本阈值 (0-255)
    double thresholdMin, thresholdMax;  // 范围阈值 (0-255)
    int adaptiveMethod;           // 自适应方法 (0=MEAN, 1=GAUSSIAN)
    int thresholdType;            // 阈值类型 (0=BINARY, 1=BINARY_INV)
    int blockSize;                // 自适应阈值块大小
    double C;                     // 自适应阈值常数

    // 形态学参数
    int morphKernelSize;          // 形态学核大小 (3-51, odd only)
    double morphThreshold;        // 智能形态学阈值 (0-255)
    int morphKernelType;          // 核类型 (0=RECT, 1=ELLIPSE, 2=CROSS)
    double edgeThreshold;         // 边缘检测阈值 (10-300)
    int separationMethod;         // 分离方法 (0=Canny, 1=Sobel, 2=Laplacian)

    // 清理参数
    int minHoleSize;              // 最小孔洞大小 (1-500)
    int fillMethod;               // 填充方法 (0=Simple, 1=Morphological, 2=Flood)
    int minFeatureSize;           // 最小特征大小 (1-1000)
    int maxFeatureSize;           // 最大特征大小 (100-5000)
    int rejectMethod;             // 拒绝方法 (0=Size-based, 1=Area+Morphology, 2=Contour-based)

    // 测量参数
    int minObjectSize;            // 最小对象大小 (1-1000)
    int maxObjectSize;            // 最大对象大小 (100-50000)
    double sensitivity;           // 检测敏感度 (0.1-1.0)

    // 参数变化检测用的前一个值
    double prevBrightness;
    double prevContrast;
    double prevClipLimit;
    int prevFlattenKernelSize;
    
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
    void renderMainInterface();

    /**
     * @brief 渲染主控制面板
     */
    void renderControlPanel();

    /**
     * @brief 渲染图像显示区域
     */
    void renderImageDisplay();

    /**
     * @brief 渲染当前模态窗口
     */
    void renderCurrentModal();

    /**
     * @brief 渲染特定功能的模态窗口
     */
    void renderLoadImageModal();
    void renderConvertGrayscaleModal();
    void renderColorSelectModal();
    void renderColorClusterModal();
    void renderColorDeconvolutionModal();
    void renderChannelOperationModal();
    void renderResetImageModal();
    void renderPreProcessingModal();
    void renderMorphologyModal();
    void renderCleanUpModal();
    void renderMeasurementsModal();
    void renderSegmentationModal();
    void renderSegmentationParameters();
    void renderBasicThresholdParameters(int startY);
    void renderRangeThresholdParameters(int startY);
    void renderAdaptiveThresholdParameters(int startY);
    void renderMorphologyParameters();
    void renderDilateErodeParameters(int startY);

    /**
     * @brief 模态窗口辅助方法
     */
    void updatePreview();
    void renderPreviewArea(int x, int y, int width, int height);

    /**
     * @brief 执行K-Means聚类
     */
    cv::Mat performKMeans(const cv::Mat& image, int k);







    // 具体的分割算法实现
    cv::Mat applyBasicThreshold(const cv::Mat& image, double threshold, int type);
    cv::Mat applyRangeThreshold(const cv::Mat& image, double minVal, double maxVal);
    cv::Mat applyAdaptiveThreshold(const cv::Mat& image, int method, int type, int blockSize, double C);
    cv::Mat applyOtsuThreshold(const cv::Mat& image);
    cv::Mat applyLocalThreshold(const cv::Mat& image);

    // 具体的形态学算法实现
    cv::Mat applyDilateUniform(const cv::Mat& image, int kernelSize, int kernelType);
    cv::Mat applyDilateSmart(const cv::Mat& image, int kernelSize, int kernelType, double threshold);
    cv::Mat applyDilateRetain(const cv::Mat& image, int kernelSize, int kernelType);
    cv::Mat applyErodeUniform(const cv::Mat& image, int kernelSize, int kernelType);
    cv::Mat applyErodeSmart(const cv::Mat& image, int kernelSize, int kernelType, double threshold);
    cv::Mat applyErodeRetain(const cv::Mat& image, int kernelSize, int kernelType);
    cv::Mat applySeparateFeatures(const cv::Mat& image);
    
    /**
     * @brief 打开文件对话框
     * @return 选择的文件路径
     */
    std::string openFileDialog();
    
    /**
     * @brief 缩放图像以适应显示区域
     * @param image 原始图像
     * @param maxWidth 最大宽度
     * @param maxHeight 最大高度
     * @return 缩放后的图像
     */
    cv::Mat scaleImageToFit(const cv::Mat& image, int maxWidth = 800, int maxHeight = 400);

    /**
     * @brief 打开模态窗口
     */
    void openModal(ModalFunction modal);

    /**
     * @brief 关闭模态窗口
     */
    void closeModal();

    /**
     * @brief 渲染模态窗口按钮
     */
    void renderModalButtons(int x, int y);

    /**
     * @brief 应用当前功能
     */
    void applyCurrentFunction();

    /**
     * @brief 应用预处理功能
     */
    void applyPreProcessingFunction(PreProcessingFunction function);

    /**
     * @brief 更新预处理预览
     */
    void updatePreProcessingPreview(PreProcessingFunction function);

    /**
     * @brief 更新颜色选择预览
     */
    void updateColorSelectPreview();

    /**
     * @brief 更新颜色聚类预览
     */
    void updateColorClusterPreview();

    /**
     * @brief 更新颜色反褶积预览
     */
    void updateColorDeconvolutionPreview();

    /**
     * @brief 更新通道操作预览
     */
    void updateChannelOperationPreview();

    /**
     * @brief 应用分割功能
     */
    void applySegmentationFunction(SegmentationFunction function);

    /**
     * @brief 更新分割预览
     */
    void updateSegmentationPreview(SegmentationFunction function);

    /**
     * @brief 应用形态学功能
     */
    void applyMorphologyFunction(MorphologyFunction function);

    /**
     * @brief 更新形态学预览
     */
    void updateMorphologyPreview(MorphologyFunction function);

    /**
     * @brief 应用清理功能
     */
    void applyCleanUpFunction(CleanUpFunction function);

    /**
     * @brief 更新清理预览
     */
    void updateCleanUpPreview(CleanUpFunction function);

    /**
     * @brief 应用测量功能
     */
    void applyMeasurementsFunction(MeasurementsFunction function);

    /**
     * @brief 更新测量预览
     */
    void updateMeasurementsPreview(MeasurementsFunction function);
};