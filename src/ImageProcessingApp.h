#pragma once

#include "ImageProcessor.h"
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

    // 通道选择
    int selected_channel;

    // 通道操作参数
    int operation_type;  // 0=add, 1=subtract, 2=multiply, 3=divide
    double operation_value;

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
        PRE_PROCESSING = 7
    };

    enum class PreProcessingFunction {
        NONE = -1,
        // CONTRAST (对比度调节)
        ADJUST_CONTRAST = 0,
        HISTOGRAM_EQUALIZATION = 1,
        FLATTEN_BACKGROUND = 2,
        // NOISE-REDUCTION (降噪处理)
        MEDIAN_FILTER = 3,
        WIENER_FILTER = 4,
        NON_LOCAL_MEANS = 5,
        // BLUR (模糊处理)
        GAUSSIAN_BLUR = 6,
        AVERAGE_BLUR = 7,
        SUM_FILTER = 8,
        GRAYSCALE_DILATE = 9,
        GRAYSCALE_ERODE = 10,
        // EDGES (突出边界)
        STDDEV_FILTER = 11,
        ENTROPY_FILTER = 12,
        GRADIENT_FILTER = 13,
        HIGHLIGHT_LINES = 14,
        // TEXTURE (突出纹理特征)
        BRIGHT_TEXTURE = 15,
        DARK_TEXTURE = 16,
        ADVANCED_TEXTURE = 17,
        SIMILARITY = 18,
        // CORRECTION (图像修正)
        SHARPEN = 19,
        FFT_FILTER = 20,
        GRAYSCALE_INTERPOLATION = 21,
        GRAYSCALE_RECONSTRUCTION = 22
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
    void renderUI();
    
    /**
     * @brief 渲染主控制面板
     */
    void renderMainControlPanel();

    /**
     * @brief 渲染图像显示区域
     */
    void renderImageDisplay();

    /**
     * @brief 渲染模态窗口
     */
    void renderModalWindow();

    /**
     * @brief 渲染特定功能的模态窗口
     */
    void renderLoadImageModal();
    void renderGrayscaleModal();
    void renderColorSelectModal();
    void renderColorClusterModal();
    void renderColorDeconvolutionModal();
    void renderChannelOperationModal();
    void renderResetImageModal();
    void renderPreProcessingModal();
    void renderPreProcessingParameters();
    void renderAdjustContrastParameters(int startY);
    void renderHistogramEqualizationParameters(int startY);
    void renderFlattenBackgroundParameters(int startY);

    /**
     * @brief 模态窗口辅助方法
     */
    void openModal(ModalFunction function);
    void closeModal();
    void updatePreview();
    void applyCurrentFunction();
    void renderPreviewArea(int x, int y, int width, int height);
    void renderModalButtons(int x, int y);

    /**
     * @brief 执行K-Means聚类
     */
    cv::Mat performKMeans(const cv::Mat& image, int k);

    /**
     * @brief 预处理功能实现方法
     */
    void applyPreProcessingFunction(PreProcessingFunction function);
    void updatePreProcessingPreview(PreProcessingFunction function);

    // 具体的预处理算法实现
    cv::Mat applyAdjustContrast(const cv::Mat& image, double brightness, double contrast);
    cv::Mat applyHistogramEqualization(const cv::Mat& image);
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