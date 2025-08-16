#include "UIComponents.h"
#include <cvui.h>
#include <iostream>

UIComponents::UIComponents() {
}

UIComponents::~UIComponents() {
}

cv::Mat UIComponents::scaleImageToFit(const cv::Mat& image, int maxWidth, int maxHeight) {
    if (image.empty()) {
        return cv::Mat();
    }

    double scaleX = (double)maxWidth / image.cols;
    double scaleY = (double)maxHeight / image.rows;
    double scale = std::min(scaleX, scaleY);

    if (scale >= 1.0) {
        return image.clone();
    }

    cv::Mat scaledImage;
    cv::resize(image, scaledImage, cv::Size(0, 0), scale, scale, cv::INTER_AREA);
    return scaledImage;
}

void UIComponents::renderPreviewArea(cv::Mat& frame, int x, int y, int width, int height, const cv::Mat& previewImage) {
    // 绘制预览区域边框
    cv::rectangle(frame, cv::Rect(x, y, width, height), cv::Scalar(200, 200, 200), 1);

    if (!previewImage.empty()) {
        cv::Mat scaledPreview = scaleImageToFit(previewImage, width - 10, height - 10);
        if (!scaledPreview.empty()) {
            int imgX = x + (width - scaledPreview.cols) / 2;
            int imgY = y + (height - scaledPreview.rows) / 2;

            // 确保图像是3通道的，以避免cvui::image的copyTo错误
            cv::Mat displayPreview;
            if (scaledPreview.channels() == 1) {
                cv::cvtColor(scaledPreview, displayPreview, cv::COLOR_GRAY2BGR);
            } else {
                displayPreview = scaledPreview.clone();
            }

            cvui::image(frame, imgX, imgY, displayPreview);
        }
    } else {
        cvui::text(frame, x + 10, y + height / 2, "No preview", 0.4);
    }
}

int UIComponents::renderModalButtons(cv::Mat& frame, int x, int y) {
    if (cvui::button(frame, x, y, 80, 30, "Apply", 0.35)) {
        return 1; // Apply clicked
    }
    
    if (cvui::button(frame, x + 100, y, 80, 30, "Cancel", 0.35)) {
        return 2; // Cancel clicked
    }
    
    return 0; // No button clicked
}

bool UIComponents::renderAdjustContrastParameters(cv::Mat& frame, int startY, int controlAreaX, 
                                                double& brightness, double& contrast,
                                                double& prevBrightness, double& prevContrast,
                                                PreProcessingFunction currentFunction) {
    int currentY = startY;
    
    cvui::text(frame, controlAreaX, currentY, "Brightness Adjustment:", 0.35);
    currentY += 20;
    cvui::trackbar(frame, controlAreaX, currentY, 200, &brightness, -100.0, 100.0);
    cvui::text(frame, controlAreaX + 210, currentY + 8, ("Value: " + std::to_string((int)brightness)).c_str(), 0.3);
    currentY += 40;
    
    cvui::text(frame, controlAreaX, currentY, "Contrast Adjustment:", 0.35);
    currentY += 20;
    cvui::trackbar(frame, controlAreaX, currentY, 200, &contrast, 0.1, 3.0);
    cvui::text(frame, controlAreaX + 210, currentY + 8, ("Value: " + std::to_string(contrast).substr(0, 4)).c_str(), 0.3);
    currentY += 40;
    
    cvui::text(frame, controlAreaX, currentY, "Brightness: " + std::to_string((int)brightness), 0.3);
    cvui::text(frame, controlAreaX, currentY + 15, "Contrast: " + std::to_string(contrast).substr(0, 4), 0.3);
    
    // 检测参数变化并自动更新预览
    bool needsUpdate = false;
    if (brightness != prevBrightness || contrast != prevContrast) {
        needsUpdate = true;
        prevBrightness = brightness;
        prevContrast = contrast;
    }
    
    if (cvui::button(frame, controlAreaX, currentY + 40, 120, 25, "Update Preview", 0.35)) {
        needsUpdate = true;
    }
    
    return needsUpdate;
}

bool UIComponents::renderHistogramEqualizationParameters(cv::Mat& frame, int startY, int controlAreaX,
                                                       int& histogramMethod, double& clipLimit,
                                                       double& prevClipLimit, PreProcessingFunction currentFunction) {
    int currentY = startY;
    bool needsUpdate = false;
    
    cvui::text(frame, controlAreaX, currentY, "Equalization Method:", 0.35);
    currentY += 25;
    
    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Global", 0.3)) {
        histogramMethod = 0;
        needsUpdate = true;
    }
    if (histogramMethod == 0) {
        cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
    }
    currentY += 30;
    
    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Adaptive (CLAHE)", 0.3)) {
        histogramMethod = 1;
        needsUpdate = true;
    }
    if (histogramMethod == 1) {
        cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
    }
    currentY += 40;
    
    // CLAHE参数只在自适应模式下显示
    if (histogramMethod == 1) {
        cvui::text(frame, controlAreaX, currentY, "CLAHE Clip Limit:", 0.35);
        currentY += 20;
        cvui::trackbar(frame, controlAreaX, currentY, 200, &clipLimit, 1.0, 40.0);
        cvui::text(frame, controlAreaX + 210, currentY + 8, ("Value: " + std::to_string(clipLimit).substr(0, 4)).c_str(), 0.3);
        currentY += 40;
        
        // 检测clipLimit变化并自动更新预览
        if (clipLimit != prevClipLimit) {
            needsUpdate = true;
            prevClipLimit = clipLimit;
        }
        
        if (cvui::button(frame, controlAreaX, currentY, 120, 25, "Update Preview", 0.35)) {
            needsUpdate = true;
        }
    }
    
    return needsUpdate;
}

bool UIComponents::renderFlattenBackgroundParameters(cv::Mat& frame, int startY, int controlAreaX,
                                                   int& flattenKernelSize, int& prevFlattenKernelSize,
                                                   PreProcessingFunction currentFunction) {
    int currentY = startY;
    
    cvui::text(frame, controlAreaX, currentY, "Kernel Size (odd numbers only):", 0.35);
    currentY += 20;
    
    // 确保核大小为奇数
    int tempKernel = flattenKernelSize;
    cvui::trackbar(frame, controlAreaX, currentY, 200, &tempKernel, 5, 51);
    
    // 强制为奇数
    if (tempKernel % 2 == 0) {
        tempKernel += 1;
    }
    flattenKernelSize = tempKernel;
    
    cvui::text(frame, controlAreaX + 210, currentY + 8, ("Size: " + std::to_string(flattenKernelSize)).c_str(), 0.3);
    currentY += 40;
    
    cvui::text(frame, controlAreaX, currentY, "Current kernel size: " + std::to_string(flattenKernelSize), 0.3);
    cvui::text(frame, controlAreaX, currentY + 15, "Larger values = more background removal", 0.25);
    
    // 检测flattenKernelSize变化并自动更新预览
    bool needsUpdate = false;
    if (flattenKernelSize != prevFlattenKernelSize) {
        needsUpdate = true;
        prevFlattenKernelSize = flattenKernelSize;
    }
    
    if (cvui::button(frame, controlAreaX, currentY + 40, 120, 25, "Update Preview", 0.35)) {
        needsUpdate = true;
    }
    
    return needsUpdate;
}

PreProcessingFunction UIComponents::renderPreProcessingFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY) {
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "Select a Pre-Processing Function:", 0.4);
    currentY += 30;

    // CONTRAST (对比度调节)
    cvui::text(frame, controlAreaX, currentY, "CONTRAST:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Adjust Contrast", 0.3)) {
        return PreProcessingFunction::ADJUST_CONTRAST;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Histogram Eq", 0.3)) {
        return PreProcessingFunction::HISTOGRAM_EQUALIZATION;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Flatten BG", 0.3)) {
        return PreProcessingFunction::FLATTEN_BACKGROUND;
    }
    currentY += 40;

    // NOISE-REDUCTION (降噪处理)
    cvui::text(frame, controlAreaX, currentY, "NOISE-REDUCTION:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Median Filter", 0.3)) {
        return PreProcessingFunction::MEDIAN_FILTER;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Wiener Filter", 0.3)) {
        return PreProcessingFunction::WIENER_FILTER;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Non-Local Means", 0.3)) {
        return PreProcessingFunction::NON_LOCAL_MEANS;
    }
    currentY += 40;

    // BLUR (模糊处理)
    cvui::text(frame, controlAreaX, currentY, "BLUR:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Gaussian Blur", 0.3)) {
        return PreProcessingFunction::GAUSSIAN_BLUR;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Average Blur", 0.3)) {
        return PreProcessingFunction::AVERAGE_BLUR;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Sum Filter", 0.3)) {
        return PreProcessingFunction::SUM_FILTER;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "GS Dilate", 0.3)) {
        return PreProcessingFunction::GRAYSCALE_DILATE;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "GS Erode", 0.3)) {
        return PreProcessingFunction::GRAYSCALE_ERODE;
    }

    return PreProcessingFunction::NONE;
}

int UIComponents::renderPreProcessingParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                              PreProcessingFunction currentFunction,
                                              double& brightness, double& contrast,
                                              int& histogramMethod, double& clipLimit, int& flattenKernelSize,
                                              double& prevBrightness, double& prevContrast,
                                              double& prevClipLimit, int& prevFlattenKernelSize) {
    int currentY = controlAreaY;

    // 显示当前选择的功能名称和返回按钮
    const char* functionNames[] = {
        "Adjust Contrast", "Histogram Equalization", "Flatten Background",
        "Median Filter", "Wiener Filter", "Non-Local Means",
        "Gaussian Blur", "Average Blur", "Sum Filter", "Grayscale Dilate", "Grayscale Erode",
        "StdDev Filter", "Entropy Filter", "Gradient Filter", "Highlight Lines",
        "Bright Texture", "Dark Texture", "Advanced Texture", "Similarity",
        "Sharpen", "FFT Filter", "Grayscale Interpolation", "Grayscale Reconstruction"
    };

    cvui::text(frame, controlAreaX, currentY, functionNames[(int)currentFunction], 0.4);

    if (cvui::button(frame, controlAreaX + 200, currentY - 5, 80, 25, "< Back", 0.3)) {
        return 1; // Back button clicked
    }
    currentY += 35;

    // 根据选择的功能显示相应的参数控制
    bool needsUpdate = false;
    switch (currentFunction) {
        case PreProcessingFunction::ADJUST_CONTRAST:
            needsUpdate = renderAdjustContrastParameters(frame, currentY, controlAreaX,
                                                       brightness, contrast, prevBrightness, prevContrast, currentFunction);
            break;
        case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
            needsUpdate = renderHistogramEqualizationParameters(frame, currentY, controlAreaX,
                                                               histogramMethod, clipLimit, prevClipLimit, currentFunction);
            break;
        case PreProcessingFunction::FLATTEN_BACKGROUND:
            needsUpdate = renderFlattenBackgroundParameters(frame, currentY, controlAreaX,
                                                           flattenKernelSize, prevFlattenKernelSize, currentFunction);
            break;
        default:
            cvui::text(frame, controlAreaX, currentY, "No parameters for this function.", 0.35);
            cvui::text(frame, controlAreaX, currentY + 25, "Click Apply to execute.", 0.35);
            break;
    }

    return needsUpdate ? 2 : 0; // 2 = update preview, 0 = no action
}

// Segmentation UI methods implementation
SegmentationFunction UIComponents::renderSegmentationFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY) {
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "Select a Segmentation Function:", 0.4);
    currentY += 30;

    // THRESHOLD (阈值标记)
    cvui::text(frame, controlAreaX, currentY, "THRESHOLD:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Basic Threshold", 0.3)) {
        return SegmentationFunction::BASIC_THRESHOLD;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Range Threshold", 0.3)) {
        return SegmentationFunction::RANGE_THRESHOLD;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Adaptive Threshold", 0.3)) {
        return SegmentationFunction::ADAPTIVE_THRESHOLD;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "EM Threshold", 0.3)) {
        return SegmentationFunction::EM_THRESHOLD;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Local Threshold", 0.3)) {
        return SegmentationFunction::LOCAL_THRESHOLD;
    }

    return SegmentationFunction::NONE;
}

int UIComponents::renderSegmentationParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                             SegmentationFunction currentFunction,
                                             double& thresholdValue, int& thresholdType,
                                             double& thresholdMin, double& thresholdMax,
                                             int& adaptiveMethod, int& blockSize, double& C) {
    int currentY = controlAreaY;

    // 显示当前选择的功能名称和返回按钮
    const char* functionNames[] = {
        "Basic Threshold", "Range Threshold", "Adaptive Threshold", "EM Threshold", "Local Threshold"
    };

    cvui::text(frame, controlAreaX, currentY, functionNames[(int)currentFunction], 0.4);

    if (cvui::button(frame, controlAreaX + 200, currentY - 5, 80, 25, "< Back", 0.3)) {
        return 1; // Back button clicked
    }
    currentY += 35;

    bool needsUpdate = false;

    // 根据选择的功能显示相应的参数控制
    switch (currentFunction) {
        case SegmentationFunction::BASIC_THRESHOLD:
            cvui::text(frame, controlAreaX, currentY, "Threshold Value:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &thresholdValue, 0.0, 255.0);
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("Value: " + std::to_string((int)thresholdValue)).c_str(), 0.3);
            currentY += 40;

            cvui::text(frame, controlAreaX, currentY, "Threshold Type:", 0.35);
            currentY += 25;
            if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Binary", 0.3)) {
                thresholdType = 0;
                needsUpdate = true;
            }
            if (thresholdType == 0) {
                cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
            }
            currentY += 30;
            if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Binary Inv", 0.3)) {
                thresholdType = 1;
                needsUpdate = true;
            }
            if (thresholdType == 1) {
                cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
            }
            break;

        case SegmentationFunction::RANGE_THRESHOLD:
            cvui::text(frame, controlAreaX, currentY, "Minimum Value:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &thresholdMin, 0.0, 255.0);
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("Min: " + std::to_string((int)thresholdMin)).c_str(), 0.3);
            currentY += 40;

            cvui::text(frame, controlAreaX, currentY, "Maximum Value:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &thresholdMax, 0.0, 255.0);
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("Max: " + std::to_string((int)thresholdMax)).c_str(), 0.3);
            break;

        case SegmentationFunction::ADAPTIVE_THRESHOLD:
            cvui::text(frame, controlAreaX, currentY, "Adaptive Method:", 0.35);
            currentY += 25;
            if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Mean", 0.3)) {
                adaptiveMethod = 0;
                needsUpdate = true;
            }
            if (adaptiveMethod == 0) {
                cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
            }
            currentY += 30;
            if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Gaussian", 0.3)) {
                adaptiveMethod = 1;
                needsUpdate = true;
            }
            if (adaptiveMethod == 1) {
                cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
            }
            currentY += 40;

            cvui::text(frame, controlAreaX, currentY, "Block Size:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &blockSize, 3, 31);
            // Ensure odd number
            if (blockSize % 2 == 0) blockSize++;
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("Size: " + std::to_string(blockSize)).c_str(), 0.3);
            currentY += 40;

            cvui::text(frame, controlAreaX, currentY, "C Value:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &C, -10.0, 10.0);
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("C: " + std::to_string(C).substr(0, 4)).c_str(), 0.3);
            break;

        default:
            cvui::text(frame, controlAreaX, currentY, "No parameters for this function.", 0.35);
            cvui::text(frame, controlAreaX, currentY + 25, "Click Apply to execute.", 0.35);
            break;
    }

    if (cvui::button(frame, controlAreaX, currentY + 60, 120, 25, "Update Preview", 0.35)) {
        needsUpdate = true;
    }

    return needsUpdate ? 2 : 0; // 2 = update preview, 0 = no action
}

// Morphology UI methods implementation
MorphologyFunction UIComponents::renderMorphologyFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY) {
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "Select a Morphology Function:", 0.4);
    currentY += 30;

    // MORPHOLOGY (形态学操作)
    cvui::text(frame, controlAreaX, currentY, "MORPHOLOGY:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Dilate", 0.3)) {
        return MorphologyFunction::DILATE;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Erode", 0.3)) {
        return MorphologyFunction::ERODE;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Opening", 0.3)) {
        return MorphologyFunction::OPENING;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Closing", 0.3)) {
        return MorphologyFunction::CLOSING;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Gradient", 0.3)) {
        return MorphologyFunction::GRADIENT;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Top Hat", 0.3)) {
        return MorphologyFunction::TOP_HAT;
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Black Hat", 0.3)) {
        return MorphologyFunction::BLACK_HAT;
    }
    currentY += 40;

    // EDGES (边界调节)
    cvui::text(frame, controlAreaX, currentY, "EDGES:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Separate Features", 0.3)) {
        return MorphologyFunction::SEPARATE_FEATURES;
    }

    return MorphologyFunction::NONE;
}

int UIComponents::renderMorphologyParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                           MorphologyFunction currentFunction,
                                           int& morphKernelSize, int& morphKernelType,
                                           double& edgeThreshold, int& separationMethod) {
    int currentY = controlAreaY;

    // 显示当前选择的功能名称和返回按钮
    const char* functionNames[] = {
        "Dilate", "Erode", "Opening", "Closing", "Gradient", "Top Hat", "Black Hat", "Separate Features"
    };

    cvui::text(frame, controlAreaX, currentY, functionNames[(int)currentFunction], 0.4);

    if (cvui::button(frame, controlAreaX + 200, currentY - 5, 80, 25, "< Back", 0.3)) {
        return 1; // Back button clicked
    }
    currentY += 35;

    bool needsUpdate = false;

    // Kernel Size
    cvui::text(frame, controlAreaX, currentY, "Kernel Size:", 0.35);
    currentY += 20;
    cvui::trackbar(frame, controlAreaX, currentY, 200, &morphKernelSize, 3, 21);
    // Ensure odd number
    if (morphKernelSize % 2 == 0) morphKernelSize++;
    cvui::text(frame, controlAreaX + 210, currentY + 8, ("Size: " + std::to_string(morphKernelSize)).c_str(), 0.3);
    currentY += 40;

    // Kernel Type
    cvui::text(frame, controlAreaX, currentY, "Kernel Type:", 0.35);
    currentY += 25;

    const char* kernelTypes[] = {"Rectangle", "Ellipse", "Cross"};
    for (int i = 0; i < 3; i++) {
        if (cvui::button(frame, controlAreaX, currentY + i * 30, 100, 25, kernelTypes[i], 0.3)) {
            morphKernelType = i;
            needsUpdate = true;
        }
        if (morphKernelType == i) {
            cvui::text(frame, controlAreaX + 110, currentY + i * 30 + 8, "<- Selected", 0.25);
        }
    }
    currentY += 100;

    cvui::text(frame, controlAreaX, currentY, ("Current: " + std::string(kernelTypes[morphKernelType]) + " kernel, size " + std::to_string(morphKernelSize)).c_str(), 0.3);
    currentY += 40;

    // Additional parameters for Separate Features
    if (currentFunction == MorphologyFunction::SEPARATE_FEATURES) {
        cvui::text(frame, controlAreaX, currentY, "Edge Detection Threshold:", 0.35);
        currentY += 20;
        cvui::trackbar(frame, controlAreaX, currentY, 200, &edgeThreshold, 10.0, 300.0);
        cvui::text(frame, controlAreaX + 210, currentY + 8, ("Threshold: " + std::to_string((int)edgeThreshold)).c_str(), 0.3);
        currentY += 40;

        cvui::text(frame, controlAreaX, currentY, "Separation Method:", 0.35);
        currentY += 25;

        const char* separationMethods[] = {"Canny", "Sobel", "Laplacian"};
        for (int i = 0; i < 3; i++) {
            if (cvui::button(frame, controlAreaX, currentY + i * 30, 100, 25, separationMethods[i], 0.3)) {
                separationMethod = i;
                needsUpdate = true;
            }
            if (separationMethod == i) {
                cvui::text(frame, controlAreaX + 110, currentY + i * 30 + 8, "<- Selected", 0.25);
            }
        }
        currentY += 100;

        cvui::text(frame, controlAreaX, currentY, ("Method: " + std::string(separationMethods[separationMethod])).c_str(), 0.3);
        currentY += 20;
    }

    if (cvui::button(frame, controlAreaX, currentY + 10, 120, 25, "Update Preview", 0.35)) {
        needsUpdate = true;
    }

    return needsUpdate ? 2 : 0; // 2 = update preview, 0 = no action
}

// CleanUp UI methods implementation
CleanUpFunction UIComponents::renderCleanUpFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY) {
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "Select a Clean-Up Function:", 0.4);
    currentY += 30;

    // REJECT (拒绝/清理)
    cvui::text(frame, controlAreaX, currentY, "REJECT:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Fill All Holes", 0.3)) {
        return CleanUpFunction::FILL_ALL_HOLES;
    }
    if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Reject Features", 0.3)) {
        return CleanUpFunction::REJECT_FEATURES;
    }

    return CleanUpFunction::NONE;
}

int UIComponents::renderCleanUpParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                        CleanUpFunction currentFunction,
                                        int& minHoleSize, int& fillMethod,
                                        int& minFeatureSize, int& maxFeatureSize, int& rejectMethod) {
    int currentY = controlAreaY;

    // 显示当前选择的功能名称和返回按钮
    const char* functionNames[] = {
        "Fill All Holes", "Reject Features"
    };

    cvui::text(frame, controlAreaX, currentY, functionNames[(int)currentFunction], 0.4);

    if (cvui::button(frame, controlAreaX + 200, currentY - 5, 80, 25, "< Back", 0.3)) {
        return 1; // Back button clicked
    }
    currentY += 35;

    bool needsUpdate = false;

    // 根据选择的功能显示相应的参数控制
    switch (currentFunction) {
        case CleanUpFunction::FILL_ALL_HOLES:
            {
                cvui::text(frame, controlAreaX, currentY, "Minimum Hole Size:", 0.35);
                currentY += 20;
                cvui::trackbar(frame, controlAreaX, currentY, 200, &minHoleSize, 1, 500);
                cvui::text(frame, controlAreaX + 210, currentY + 8, ("Size: " + std::to_string(minHoleSize)).c_str(), 0.3);
                currentY += 40;

                cvui::text(frame, controlAreaX, currentY, "Fill Method:", 0.35);
                currentY += 25;

                const char* fillMethods[] = {"Simple Fill", "Morphological Fill", "Flood Fill"};
                for (int i = 0; i < 3; i++) {
                    if (cvui::button(frame, controlAreaX, currentY + i * 30, 120, 25, fillMethods[i], 0.3)) {
                        fillMethod = i;
                        needsUpdate = true;
                    }
                    if (fillMethod == i) {
                        cvui::text(frame, controlAreaX + 130, currentY + i * 30 + 8, "<- Selected", 0.25);
                    }
                }
                currentY += 100;

                cvui::text(frame, controlAreaX, currentY, ("Method: " + std::string(fillMethods[fillMethod])).c_str(), 0.3);
            }
            break;

        case CleanUpFunction::REJECT_FEATURES:
            {
                cvui::text(frame, controlAreaX, currentY, "Minimum Feature Size:", 0.35);
                currentY += 20;
                cvui::trackbar(frame, controlAreaX, currentY, 200, &minFeatureSize, 1, 1000);
                cvui::text(frame, controlAreaX + 210, currentY + 8, ("Min: " + std::to_string(minFeatureSize)).c_str(), 0.3);
                currentY += 40;

                cvui::text(frame, controlAreaX, currentY, "Maximum Feature Size:", 0.35);
                currentY += 20;
                cvui::trackbar(frame, controlAreaX, currentY, 200, &maxFeatureSize, 100, 5000);
                cvui::text(frame, controlAreaX + 210, currentY + 8, ("Max: " + std::to_string(maxFeatureSize)).c_str(), 0.3);
                currentY += 40;

                cvui::text(frame, controlAreaX, currentY, "Reject Method:", 0.35);
                currentY += 25;

                const char* rejectMethods[] = {"Size-based", "Area + Morphology", "Contour-based"};
                for (int i = 0; i < 3; i++) {
                    if (cvui::button(frame, controlAreaX, currentY + i * 30, 120, 25, rejectMethods[i], 0.3)) {
                        rejectMethod = i;
                        needsUpdate = true;
                    }
                    if (rejectMethod == i) {
                        cvui::text(frame, controlAreaX + 130, currentY + i * 30 + 8, "<- Selected", 0.25);
                    }
                }
                currentY += 100;

                cvui::text(frame, controlAreaX, currentY, ("Method: " + std::string(rejectMethods[rejectMethod])).c_str(), 0.3);
            }
            break;

        default:
            cvui::text(frame, controlAreaX, currentY, "No parameters for this function.", 0.35);
            cvui::text(frame, controlAreaX, currentY + 25, "Click Apply to execute.", 0.35);
            break;
    }

    if (cvui::button(frame, controlAreaX, currentY + 40, 120, 25, "Update Preview", 0.35)) {
        needsUpdate = true;
    }

    return needsUpdate ? 2 : 0; // 2 = update preview, 0 = no action
}

// Measurements UI methods implementation
MeasurementsFunction UIComponents::renderMeasurementsFunctionSelection(cv::Mat& frame, int controlAreaX, int controlAreaY) {
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "Select a Measurements Function:", 0.4);
    currentY += 30;

    // PER IMAGE (每图像)
    cvui::text(frame, controlAreaX, currentY, "PER IMAGE:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Count Objects", 0.3)) {
        return MeasurementsFunction::COUNT;
    }

    return MeasurementsFunction::NONE;
}

int UIComponents::renderMeasurementsParameters(cv::Mat& frame, int controlAreaX, int controlAreaY,
                                             MeasurementsFunction currentFunction,
                                             int& minObjectSize, int& maxObjectSize, double& sensitivity) {
    int currentY = controlAreaY;

    // 显示当前选择的功能名称和返回按钮
    const char* functionNames[] = {
        "Count Objects"
    };

    cvui::text(frame, controlAreaX, currentY, functionNames[(int)currentFunction], 0.4);

    if (cvui::button(frame, controlAreaX + 200, currentY - 5, 80, 25, "< Back", 0.3)) {
        return 1; // Back button clicked
    }
    currentY += 35;

    bool needsUpdate = false;

    // 根据选择的功能显示相应的参数控制
    switch (currentFunction) {
        case MeasurementsFunction::COUNT:
            cvui::text(frame, controlAreaX, currentY, "Minimum Object Size:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &minObjectSize, 1, 1000);
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("Min: " + std::to_string(minObjectSize)).c_str(), 0.3);
            currentY += 40;

            cvui::text(frame, controlAreaX, currentY, "Maximum Object Size:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &maxObjectSize, 100, 50000);
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("Max: " + std::to_string(maxObjectSize)).c_str(), 0.3);
            currentY += 40;

            cvui::text(frame, controlAreaX, currentY, "Detection Sensitivity:", 0.35);
            currentY += 20;
            cvui::trackbar(frame, controlAreaX, currentY, 200, &sensitivity, 0.1, 1.0);
            cvui::text(frame, controlAreaX + 210, currentY + 8, ("Sensitivity: " + std::to_string(sensitivity).substr(0, 4)).c_str(), 0.3);
            currentY += 40;

            cvui::text(frame, controlAreaX, currentY, "Higher sensitivity = detect more objects", 0.3);
            cvui::text(frame, controlAreaX, currentY + 15, "Lower sensitivity = detect fewer objects", 0.3);
            break;

        default:
            cvui::text(frame, controlAreaX, currentY, "No parameters for this function.", 0.35);
            cvui::text(frame, controlAreaX, currentY + 25, "Click Apply to execute.", 0.35);
            break;
    }

    if (cvui::button(frame, controlAreaX, currentY + 50, 120, 25, "Update Preview", 0.35)) {
        needsUpdate = true;
    }

    return needsUpdate ? 2 : 0; // 2 = update preview, 0 = no action
}

void UIComponents::renderMeasurementResults(cv::Mat& frame, int x, int y, int width, int height, const MeasurementResult& result) {
    // 绘制结果区域边框
    cv::rectangle(frame, cv::Rect(x, y, width, height), cv::Scalar(200, 200, 200), 1);

    cvui::text(frame, x + 10, y + 20, "Measurement Results:", 0.4);

    int currentY = y + 50;

    // 显示对象计数
    cvui::text(frame, x + 10, currentY, ("Objects Found: " + std::to_string(result.objectCount)).c_str(), 0.35);
    currentY += 25;

    // 显示总面积
    cvui::text(frame, x + 10, currentY, ("Total Area: " + std::to_string(static_cast<int>(result.totalArea)) + " px²").c_str(), 0.35);
    currentY += 25;

    // 显示平均大小
    if (result.objectCount > 0) {
        cvui::text(frame, x + 10, currentY, ("Average Size: " + std::to_string(static_cast<int>(result.averageSize)) + " px²").c_str(), 0.35);
        currentY += 25;
    }

    // 显示详细信息
    if (!result.details.empty()) {
        std::istringstream iss(result.details);
        std::string line;
        while (std::getline(iss, line) && currentY < y + height - 20) {
            cvui::text(frame, x + 10, currentY, line.c_str(), 0.3);
            currentY += 20;
        }
    }
}
