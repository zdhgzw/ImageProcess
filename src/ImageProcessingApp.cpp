#define CVUI_IMPLEMENTATION
#include "ImageProcessingApp.h"
#include "PreProcessing.h"
#include "UIComponents.h"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

ImageProcessingApp::ImageProcessingApp() : windowName("Image Processing Application") {
    initializeUI();
    
    // 初始化模态窗口状态
    currentModal = ModalFunction::NONE;
    currentPreProcessingFunction = PreProcessingFunction::NONE;
    currentSegmentationFunction = SegmentationFunction::NONE;
    currentMorphologyFunction = MorphologyFunction::NONE;
    
    // 初始化参数
    hue_min = 0; hue_max = 179;
    sat_min = 0; sat_max = 255;
    val_min = 0; val_max = 255;
    
    k_clusters = 3;
    deconvolution_channel = 0;
    operation_type = 0;
    operation_value = 1.0;
    
    // 预处理参数
    brightness = 0.0;
    contrast = 1.0;
    prevBrightness = brightness - 1;
    prevContrast = contrast - 1;
    
    histogramMethod = 0;
    clipLimit = 2.0;
    prevClipLimit = clipLimit - 1;
    
    flattenKernelSize = 15;
    prevFlattenKernelSize = flattenKernelSize - 2;
    
    // 分割参数
    thresholdValue = 127.0;
    thresholdType = 0;
    thresholdMin = 50.0;
    thresholdMax = 200.0;
    adaptiveMethod = 0;
    blockSize = 11;
    C = 2.0;
    
    // 形态学参数
    morphKernelSize = 5;
    morphKernelType = 1; // ELLIPSE
    morphThreshold = 127.0;
    edgeThreshold = 100.0;
    separationMethod = 0; // Canny

    // 清理参数
    minHoleSize = 50;
    fillMethod = 0;
    minFeatureSize = 10;
    maxFeatureSize = 1000;
    rejectMethod = 0;

    // 测量参数
    minObjectSize = 10;
    maxObjectSize = 10000;
    sensitivity = 0.5;

    isProcessing = false;
}

ImageProcessingApp::~ImageProcessingApp() {
    cv::destroyAllWindows();
}

void ImageProcessingApp::initialize() {
    cvui::init(windowName);
}

void ImageProcessingApp::run() {
    while (true) {
        frame = cv::Mat(windowHeight, windowWidth, CV_8UC3, cv::Scalar(49, 52, 49));
        
        // 渲染主界面
        renderMainInterface();
        
        // 渲染模态窗口
        if (currentModal != ModalFunction::NONE) {
            renderCurrentModal();
        }
        
        cvui::update();
        cv::imshow(windowName, frame);
        
        if (cv::waitKey(20) == 27) { // ESC键退出
            break;
        }
    }
}

void ImageProcessingApp::initializeUI() {
    windowWidth = 1200;
    windowHeight = 800;
    
    imageDisplayX = 20;
    imageDisplayY = 20;
    imageDisplayWidth = 800;
    imageDisplayHeight = 600;
    
    controlPanelX = imageDisplayX + imageDisplayWidth + 20;
    controlPanelY = imageDisplayY;
    controlPanelWidth = windowWidth - controlPanelX - 20;
    
    // 模态窗口参数
    modalWindowWidth = 900;
    modalWindowHeight = 600;
    modalWindowX = (windowWidth - modalWindowWidth) / 2;
    modalWindowY = (windowHeight - modalWindowHeight) / 2;
    
    previewAreaWidth = 400;
    previewAreaHeight = 300;
    controlAreaX = modalWindowX + previewAreaWidth + 30;
    controlAreaY = modalWindowY + 40;
}

void ImageProcessingApp::renderMainInterface() {
    // 标题
    cvui::text(frame, 20, 5, "Image Processing Application", 0.6);
    
    // 图像显示区域
    renderImageDisplay();
    
    // 控制面板
    renderControlPanel();
}

void ImageProcessingApp::renderImageDisplay() {
    cv::rectangle(frame, cv::Rect(imageDisplayX, imageDisplayY, imageDisplayWidth, imageDisplayHeight),
                  cv::Scalar(200, 200, 200), 2);

    if (processor.hasImage()) {
        cv::Mat displayImg = processor.getDisplayImage();
        if (!displayImg.empty()) {
            cv::Mat scaledImg = scaleImageToFit(displayImg);
            if (!scaledImg.empty()) {
                int imgX = imageDisplayX + (imageDisplayWidth - scaledImg.cols) / 2;
                int imgY = imageDisplayY + (imageDisplayHeight - scaledImg.rows) / 2;

                // 确保图像是3通道的，以避免cvui::image的copyTo错误
                cv::Mat freshImage;
                if (scaledImg.channels() == 1) {
                    cv::cvtColor(scaledImg, freshImage, cv::COLOR_GRAY2BGR);
                } else {
                    freshImage = scaledImg.clone();
                }

                cvui::image(frame, imgX, imgY, freshImage);
            }
        }
    } else {
        cvui::text(frame, imageDisplayX + imageDisplayWidth/2 - 50, imageDisplayY + imageDisplayHeight/2, "No image loaded", 0.35);
    }
}

void ImageProcessingApp::renderControlPanel() {
    int currentY = controlPanelY;
    
    cvui::text(frame, controlPanelX, currentY, "Control Panel", 0.5);
    currentY += 40;
    
    // 图像加载按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Load Image", 0.35)) {
        openModal(ModalFunction::LOAD_IMAGE);
    }
    currentY += 40;
    
    // 基本操作按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Convert to Grayscale", 0.35)) {
        openModal(ModalFunction::CONVERT_GRAYSCALE);
    }
    currentY += 40;
    
    // 颜色处理按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Color Select", 0.35)) {
        openModal(ModalFunction::COLOR_SELECT);
    }
    currentY += 40;
    
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Color Cluster", 0.35)) {
        openModal(ModalFunction::COLOR_CLUSTER);
    }
    currentY += 40;
    
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Color Deconvolution", 0.35)) {
        openModal(ModalFunction::COLOR_DECONVOLUTION);
    }
    currentY += 40;
    
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Channel Operation", 0.35)) {
        openModal(ModalFunction::CHANNEL_OPERATION);
    }
    currentY += 40;
    
    // 预处理按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Pre-Processing", 0.35)) {
        openModal(ModalFunction::PRE_PROCESSING);
    }
    currentY += 40;
    
    // 分割按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Segmentation", 0.35)) {
        openModal(ModalFunction::SEGMENTATION);
    }
    currentY += 40;
    
    // 形态学按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Morphology", 0.35)) {
        openModal(ModalFunction::MORPHOLOGY);
    }
    currentY += 40;

    // 清理按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Clean-Up", 0.35)) {
        openModal(ModalFunction::CLEAN_UP);
    }
    currentY += 40;

    // 测量按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Measurements", 0.35)) {
        openModal(ModalFunction::MEASUREMENTS);
    }
    currentY += 40;

    // 重置按钮
    if (cvui::button(frame, controlPanelX, currentY, 150, 30, "Reset Image", 0.35)) {
        openModal(ModalFunction::RESET_IMAGE);
    }
}

void ImageProcessingApp::renderCurrentModal() {
    switch (currentModal) {
        case ModalFunction::LOAD_IMAGE:
            renderLoadImageModal();
            break;
        case ModalFunction::CONVERT_GRAYSCALE:
            renderConvertGrayscaleModal();
            break;
        case ModalFunction::COLOR_SELECT:
            renderColorSelectModal();
            break;
        case ModalFunction::COLOR_CLUSTER:
            renderColorClusterModal();
            break;
        case ModalFunction::COLOR_DECONVOLUTION:
            renderColorDeconvolutionModal();
            break;
        case ModalFunction::CHANNEL_OPERATION:
            renderChannelOperationModal();
            break;
        case ModalFunction::RESET_IMAGE:
            renderResetImageModal();
            break;
        case ModalFunction::PRE_PROCESSING:
            renderPreProcessingModal();
            break;
        case ModalFunction::SEGMENTATION:
            renderSegmentationModal();
            break;
        case ModalFunction::MORPHOLOGY:
            renderMorphologyModal();
            break;
        case ModalFunction::CLEAN_UP:
            renderCleanUpModal();
            break;
        case ModalFunction::MEASUREMENTS:
            renderMeasurementsModal();
            break;
        default:
            break;
    }
}

void ImageProcessingApp::openModal(ModalFunction modal) {
    currentModal = modal;

    // 重置相关状态
    switch (modal) {
        case ModalFunction::PRE_PROCESSING:
            currentPreProcessingFunction = PreProcessingFunction::NONE;
            break;
        case ModalFunction::SEGMENTATION:
            currentSegmentationFunction = SegmentationFunction::NONE;
            break;
        case ModalFunction::MORPHOLOGY:
            currentMorphologyFunction = MorphologyFunction::NONE;
            break;
        case ModalFunction::CLEAN_UP:
            currentCleanUpFunction = CleanUpFunction::NONE;
            break;
        case ModalFunction::MEASUREMENTS:
            currentMeasurementsFunction = MeasurementsFunction::NONE;
            break;
        default:
            break;
    }
}

void ImageProcessingApp::closeModal() {
    currentModal = ModalFunction::NONE;
    currentPreProcessingFunction = PreProcessingFunction::NONE;
    currentSegmentationFunction = SegmentationFunction::NONE;
    currentMorphologyFunction = MorphologyFunction::NONE;
    currentCleanUpFunction = CleanUpFunction::NONE;
    currentMeasurementsFunction = MeasurementsFunction::NONE;
    previewImage = cv::Mat();
}

cv::Mat ImageProcessingApp::scaleImageToFit(const cv::Mat& image, int maxWidth, int maxHeight) {
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

void ImageProcessingApp::renderLoadImageModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Load Image", 0.4);

    cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Select an image file to load:", 0.4);

    if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 150, 30, "Browse...", 0.35)) {
        std::string imagePath = openFileDialog();
        if (!imagePath.empty()) {
            if (processor.loadImage(imagePath)) {
                std::cout << "Image loaded successfully: " << imagePath << std::endl;
                closeModal();
            } else {
                std::cout << "Failed to load image: " << imagePath << std::endl;
            }
        }
    }

    if (cvui::button(frame, modalWindowX + 200, modalWindowY + 80, 80, 30, "Cancel", 0.35)) {
        closeModal();
    }
}

void ImageProcessingApp::renderConvertGrayscaleModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Convert to Grayscale", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Convert the current image to grayscale?", 0.4);

    if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Convert", 0.35)) {
        processor.convertToGrayscale();
        std::cout << "convertToGrayscale success" << std::endl;
        closeModal();
    }

    if (cvui::button(frame, modalWindowX + 120, modalWindowY + 80, 80, 30, "Cancel", 0.35)) {
        closeModal();
    }
}

void ImageProcessingApp::renderResetImageModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Reset Image", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "No image to reset.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Reset the image to its original state?", 0.4);

    if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Reset", 0.35)) {
        processor.resetToOriginal();
        std::cout << "Image reset to original" << std::endl;
        closeModal();
    }

    if (cvui::button(frame, modalWindowX + 120, modalWindowY + 80, 80, 30, "Cancel", 0.35)) {
        closeModal();
    }
}

void ImageProcessingApp::renderPreProcessingModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Pre-Processing Functions", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 初始化预览图像（如果还没有预览图像）
    if (previewImage.empty() && processor.hasImage()) {
        previewImage = processor.getCurrentImage().clone();
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    if (currentPreProcessingFunction == PreProcessingFunction::NONE) {
        // 功能选择界面
        PreProcessingFunction selectedFunction = UIComponents::renderPreProcessingFunctionSelection(frame, controlAreaX, controlAreaY);
        if (selectedFunction != PreProcessingFunction::NONE) {
            currentPreProcessingFunction = selectedFunction;

            // 重置参数到默认值
            switch (selectedFunction) {
                case PreProcessingFunction::ADJUST_CONTRAST:
                    brightness = 0.0;
                    contrast = 1.0;
                    prevBrightness = brightness - 1;
                    prevContrast = contrast - 1;
                    break;
                case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
                    histogramMethod = 0;
                    clipLimit = 2.0;
                    break;
                case PreProcessingFunction::FLATTEN_BACKGROUND:
                    flattenKernelSize = 15;
                    prevFlattenKernelSize = flattenKernelSize - 2;
                    break;
                default:
                    break;
            }

            updatePreProcessingPreview(currentPreProcessingFunction);
        }
    } else {
        // 参数控制界面
        int result = UIComponents::renderPreProcessingParameters(frame, controlAreaX, controlAreaY, currentPreProcessingFunction,
                                                               brightness, contrast, histogramMethod, clipLimit, flattenKernelSize,
                                                               prevBrightness, prevContrast, prevClipLimit, prevFlattenKernelSize);

        if (result == 1) {
            // Back button clicked
            currentPreProcessingFunction = PreProcessingFunction::NONE;
        } else if (result == 2) {
            // Update preview
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderModalButtons(int x, int y) {
    int result = UIComponents::renderModalButtons(frame, x, y);

    if (result == 1) {
        // Apply button clicked
        std::cout << "DEBUG: Apply button clicked! currentModal = " << (int)currentModal
                  << ", currentPreProcessingFunction = " << (int)currentPreProcessingFunction << std::endl;
        applyCurrentFunction();
        std::cout << "DEBUG: applyCurrentFunction() completed, closing modal" << std::endl;
        closeModal();
        std::cout << "DEBUG: Modal closed" << std::endl;
    } else if (result == 2) {
        // Cancel button clicked
        closeModal();
    }
}

void ImageProcessingApp::applyCurrentFunction() {
    switch (currentModal) {
        case ModalFunction::PRE_PROCESSING:
            std::cout << "DEBUG: PRE_PROCESSING case reached, currentPreProcessingFunction = " << (int)currentPreProcessingFunction << std::endl;
            if (currentPreProcessingFunction != PreProcessingFunction::NONE) {
                std::cout << "DEBUG: Applying pre-processing function using direct ImageProcessor methods..." << std::endl;

                // 直接调用ImageProcessor的方法，模仿其他工作功能的模式
                switch (currentPreProcessingFunction) {
                    case PreProcessingFunction::ADJUST_CONTRAST:
                        processor.adjustContrast(brightness, contrast);
                        std::cout << "Applied contrast adjustment via ImageProcessor: brightness=" << brightness << ", contrast=" << contrast << std::endl;
                        break;
                    case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
                        processor.applyHistogramEqualization(histogramMethod, clipLimit);
                        std::cout << "Applied histogram equalization via ImageProcessor: method=" << (histogramMethod == 0 ? "global" : "adaptive")
                                  << (histogramMethod == 1 ? ", clip limit=" + std::to_string(clipLimit) : "") << std::endl;
                        break;
                    case PreProcessingFunction::FLATTEN_BACKGROUND:
                        processor.flattenBackground(flattenKernelSize);
                        std::cout << "Applied background flattening via ImageProcessor with kernel size=" << flattenKernelSize << std::endl;
                        break;
                    default:
                        std::cout << "DEBUG: Using fallback method for function " << (int)currentPreProcessingFunction << std::endl;
                        applyPreProcessingFunction(currentPreProcessingFunction);
                        break;
                }

                std::cout << "DEBUG: Pre-processing function applied successfully" << std::endl;
            } else {
                std::cout << "DEBUG: currentPreProcessingFunction is NONE, skipping application" << std::endl;
            }
            break;
        case ModalFunction::COLOR_SELECT:
            processor.colorSelect(hue_min, hue_max, sat_min, sat_max, val_min, val_max);
            std::cout << "Applied color selection with HSV ranges" << std::endl;
            break;
        case ModalFunction::COLOR_CLUSTER:
            processor.colorCluster(k_clusters);
            std::cout << "Applied color clustering with k=" << k_clusters << std::endl;
            break;
        case ModalFunction::COLOR_DECONVOLUTION:
            processor.colorDeconvolution(deconvolution_channel);
            std::cout << "Applied color deconvolution on channel " << deconvolution_channel << std::endl;
            break;
        case ModalFunction::CHANNEL_OPERATION:
            {
                const char* operations[] = {"add", "subtract", "multiply", "divide"};
                processor.channelOperation(operations[operation_type], operation_value);
                std::cout << "Applied channel operation: " << operations[operation_type] << " " << operation_value << std::endl;
            }
            break;
        case ModalFunction::SEGMENTATION:
            if (currentSegmentationFunction != SegmentationFunction::NONE) {
                applySegmentationFunction(currentSegmentationFunction);
                std::cout << "Applied segmentation function: " << (int)currentSegmentationFunction << std::endl;
            }
            break;
        case ModalFunction::MORPHOLOGY:
            if (currentMorphologyFunction != MorphologyFunction::NONE) {
                applyMorphologyFunction(currentMorphologyFunction);
                std::cout << "Applied morphology function: " << (int)currentMorphologyFunction << std::endl;
            }
            break;
        case ModalFunction::CLEAN_UP:
            if (currentCleanUpFunction != CleanUpFunction::NONE) {
                applyCleanUpFunction(currentCleanUpFunction);
                std::cout << "Applied clean-up function: " << (int)currentCleanUpFunction << std::endl;
            }
            break;
        case ModalFunction::MEASUREMENTS:
            if (currentMeasurementsFunction != MeasurementsFunction::NONE) {
                applyMeasurementsFunction(currentMeasurementsFunction);
                std::cout << "Applied measurements function: " << (int)currentMeasurementsFunction << std::endl;
            }
            break;
        default:
            break;
    }
}

void ImageProcessingApp::applyPreProcessingFunction(PreProcessingFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    try {
        cv::Mat result;
        cv::Mat currentImage = processor.getCurrentImage();

        // 准备参数数组
        std::vector<double> params;

        switch (function) {
            case PreProcessingFunction::ADJUST_CONTRAST:
                params = {brightness, contrast};
                result = PreProcessing::applyFunction(currentImage, function, params);
                std::cout << "Applied contrast adjustment: brightness=" << brightness << ", contrast=" << contrast << std::endl;
                break;
            case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
                params = {(double)histogramMethod, clipLimit};
                result = PreProcessing::applyFunction(currentImage, function, params);
                std::cout << "Applied histogram equalization: method=" << (histogramMethod == 0 ? "global" : "adaptive")
                          << (histogramMethod == 1 ? ", clip limit=" + std::to_string(clipLimit) : "") << std::endl;
                break;
            case PreProcessingFunction::FLATTEN_BACKGROUND:
                params = {(double)flattenKernelSize};
                result = PreProcessing::applyFunction(currentImage, function, params);
                std::cout << "Applied background flattening with kernel size=" << flattenKernelSize << std::endl;
                break;
            default:
                // 对于其他功能，使用默认参数
                result = PreProcessing::applyFunction(currentImage, function, {});
                std::cout << "Applied pre-processing function: " << (int)function << std::endl;
                break;
        }

        if (!result.empty()) {
            std::cout << "DEBUG: Processing result - empty: " << result.empty()
                      << ", size: " << result.cols << "x" << result.rows
                      << ", channels: " << result.channels() << std::endl;

            // 更新处理器中的图像 - 使用新的专用方法
            processor.applyPreProcessedImage(result);
        }

    } catch (const std::exception& e) {
        std::cout << "Error applying pre-processing function: " << e.what() << std::endl;
    }
}

void ImageProcessingApp::updatePreProcessingPreview(PreProcessingFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();

    try {
        // 准备参数数组
        std::vector<double> params;

        switch (function) {
            case PreProcessingFunction::ADJUST_CONTRAST:
                params = {brightness, contrast};
                tempImage = PreProcessing::applyFunction(tempImage, function, params);
                break;
            case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
                params = {(double)histogramMethod, clipLimit};
                tempImage = PreProcessing::applyFunction(tempImage, function, params);
                break;
            case PreProcessingFunction::FLATTEN_BACKGROUND:
                params = {(double)flattenKernelSize};
                tempImage = PreProcessing::applyFunction(tempImage, function, params);
                break;
            default:
                // 对于其他功能，使用默认参数
                tempImage = PreProcessing::applyFunction(tempImage, function, {});
                break;
        }

        if (!tempImage.empty()) {
            previewImage = tempImage.clone();
        }

    } catch (const std::exception& e) {
        std::cout << "Error updating pre-processing preview: " << e.what() << std::endl;
    }
}

// Color processing preview methods
void ImageProcessingApp::updateColorSelectPreview() {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();
    cv::Mat hsv, mask, result;
    cv::cvtColor(tempImage, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(hue_min, sat_min, val_min);
    cv::Scalar upper(hue_max, sat_max, val_max);
    cv::inRange(hsv, lower, upper, mask);

    tempImage.copyTo(result, mask);
    previewImage = result.clone();
}

void ImageProcessingApp::updateColorClusterPreview() {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();
    previewImage = processor.performKMeans(tempImage, k_clusters);
}

void ImageProcessingApp::updateColorDeconvolutionPreview() {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();
    std::vector<cv::Mat> channels;
    cv::split(tempImage, channels);

    if (deconvolution_channel >= 0 && deconvolution_channel < channels.size()) {
        cv::Mat result;
        cv::cvtColor(channels[deconvolution_channel], result, cv::COLOR_GRAY2BGR);
        previewImage = result.clone();
    }
}

void ImageProcessingApp::updateChannelOperationPreview() {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();
    cv::Mat result;

    const char* operations[] = {"add", "subtract", "multiply", "divide"};
    if (operation_type >= 0 && operation_type < 4) {
        if (operations[operation_type] == std::string("add")) {
            cv::add(tempImage, cv::Scalar::all(operation_value), result);
        } else if (operations[operation_type] == std::string("subtract")) {
            cv::subtract(tempImage, cv::Scalar::all(operation_value), result);
        } else if (operations[operation_type] == std::string("multiply")) {
            cv::multiply(tempImage, cv::Scalar::all(operation_value), result);
        } else if (operations[operation_type] == std::string("divide")) {
            cv::divide(tempImage, cv::Scalar::all(operation_value), result);
        }
        previewImage = result.clone();
    }
}

// Segmentation methods
void ImageProcessingApp::applySegmentationFunction(SegmentationFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    try {
        cv::Mat result;
        cv::Mat currentImage = processor.getCurrentImage();

        // 准备参数数组
        std::vector<double> params;

        switch (function) {
            case SegmentationFunction::BASIC_THRESHOLD:
                params = {thresholdValue, (double)thresholdType};
                result = Segmentation::applyFunction(currentImage, function, params);
                std::cout << "Applied basic threshold: value=" << thresholdValue << ", type=" << thresholdType << std::endl;
                break;
            case SegmentationFunction::RANGE_THRESHOLD:
                params = {thresholdMin, thresholdMax};
                result = Segmentation::applyFunction(currentImage, function, params);
                std::cout << "Applied range threshold: min=" << thresholdMin << ", max=" << thresholdMax << std::endl;
                break;
            case SegmentationFunction::ADAPTIVE_THRESHOLD:
                params = {(double)adaptiveMethod, (double)thresholdType, (double)blockSize, C};
                result = Segmentation::applyFunction(currentImage, function, params);
                std::cout << "Applied adaptive threshold: method=" << adaptiveMethod << ", blockSize=" << blockSize << ", C=" << C << std::endl;
                break;
            default:
                result = Segmentation::applyFunction(currentImage, function, {});
                std::cout << "Applied segmentation function: " << (int)function << std::endl;
                break;
        }

        if (!result.empty()) {
            processor.setCurrentImage(result);
        }

    } catch (const std::exception& e) {
        std::cout << "Error applying segmentation function: " << e.what() << std::endl;
    }
}

void ImageProcessingApp::updateSegmentationPreview(SegmentationFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();

    try {
        std::vector<double> params;

        switch (function) {
            case SegmentationFunction::BASIC_THRESHOLD:
                params = {thresholdValue, (double)thresholdType};
                tempImage = Segmentation::applyFunction(tempImage, function, params);
                break;
            case SegmentationFunction::RANGE_THRESHOLD:
                params = {thresholdMin, thresholdMax};
                tempImage = Segmentation::applyFunction(tempImage, function, params);
                break;
            case SegmentationFunction::ADAPTIVE_THRESHOLD:
                params = {(double)adaptiveMethod, (double)thresholdType, (double)blockSize, C};
                tempImage = Segmentation::applyFunction(tempImage, function, params);
                break;
            default:
                tempImage = Segmentation::applyFunction(tempImage, function, {});
                break;
        }

        if (!tempImage.empty()) {
            previewImage = tempImage.clone();
        }

    } catch (const std::exception& e) {
        std::cout << "Error updating segmentation preview: " << e.what() << std::endl;
    }
}

// Morphology methods
void ImageProcessingApp::applyMorphologyFunction(MorphologyFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    try {
        cv::Mat result;
        cv::Mat currentImage = processor.getCurrentImage();

        std::vector<double> params = {(double)morphKernelSize, (double)morphKernelType, edgeThreshold, (double)separationMethod};
        result = Morphology::applyFunction(currentImage, function, params);

        std::cout << "Applied morphology function: " << (int)function
                  << ", kernelSize=" << morphKernelSize << ", kernelType=" << morphKernelType;
        if (function == MorphologyFunction::SEPARATE_FEATURES) {
            std::cout << ", edgeThreshold=" << edgeThreshold << ", separationMethod=" << separationMethod;
        }
        std::cout << std::endl;

        if (!result.empty()) {
            processor.setCurrentImage(result);
        }

    } catch (const std::exception& e) {
        std::cout << "Error applying morphology function: " << e.what() << std::endl;
    }
}

void ImageProcessingApp::updateMorphologyPreview(MorphologyFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();

    try {
        std::vector<double> params = {(double)morphKernelSize, (double)morphKernelType, edgeThreshold, (double)separationMethod};
        tempImage = Morphology::applyFunction(tempImage, function, params);

        if (!tempImage.empty()) {
            previewImage = tempImage.clone();
        }

    } catch (const std::exception& e) {
        std::cout << "Error updating morphology preview: " << e.what() << std::endl;
    }
}

// CleanUp methods
void ImageProcessingApp::applyCleanUpFunction(CleanUpFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    try {
        cv::Mat result;
        cv::Mat currentImage = processor.getCurrentImage();

        std::vector<double> params;

        switch (function) {
            case CleanUpFunction::FILL_ALL_HOLES:
                params = {(double)minHoleSize, (double)fillMethod};
                result = CleanUp::applyFunction(currentImage, function, params);
                std::cout << "Applied fill all holes: minHoleSize=" << minHoleSize << ", fillMethod=" << fillMethod << std::endl;
                break;
            case CleanUpFunction::REJECT_FEATURES:
                params = {(double)minFeatureSize, (double)maxFeatureSize, (double)rejectMethod};
                result = CleanUp::applyFunction(currentImage, function, params);
                std::cout << "Applied reject features: minFeatureSize=" << minFeatureSize
                          << ", maxFeatureSize=" << maxFeatureSize << ", rejectMethod=" << rejectMethod << std::endl;
                break;
            default:
                result = CleanUp::applyFunction(currentImage, function, {});
                std::cout << "Applied clean-up function: " << (int)function << std::endl;
                break;
        }

        if (!result.empty()) {
            processor.setCurrentImage(result);
        }

    } catch (const std::exception& e) {
        std::cout << "Error applying clean-up function: " << e.what() << std::endl;
    }
}

void ImageProcessingApp::updateCleanUpPreview(CleanUpFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();

    try {
        std::vector<double> params;

        switch (function) {
            case CleanUpFunction::FILL_ALL_HOLES:
                params = {(double)minHoleSize, (double)fillMethod};
                tempImage = CleanUp::applyFunction(tempImage, function, params);
                break;
            case CleanUpFunction::REJECT_FEATURES:
                params = {(double)minFeatureSize, (double)maxFeatureSize, (double)rejectMethod};
                tempImage = CleanUp::applyFunction(tempImage, function, params);
                break;
            default:
                tempImage = CleanUp::applyFunction(tempImage, function, {});
                break;
        }

        if (!tempImage.empty()) {
            previewImage = tempImage.clone();
        }

    } catch (const std::exception& e) {
        std::cout << "Error updating clean-up preview: " << e.what() << std::endl;
    }
}

#ifdef _WIN32
std::string ImageProcessingApp::openFileDialog() {
    OPENFILENAME ofn;
    char szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.tiff;*.tif\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        return std::string(szFile);
    }

    return "";
}
#else
std::string ImageProcessingApp::openFileDialog() {
    // Linux/Mac implementation would go here
    return "";
}
#endif

// 简化的其他模态窗口实现
void ImageProcessingApp::renderColorSelectModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Color Select", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 初始化预览图像（如果还没有预览图像）
    if (previewImage.empty() && processor.hasImage()) {
        previewImage = processor.getCurrentImage().clone();
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "HSV Color Range Selection:", 0.4);
    currentY += 30;

    // Hue range
    cvui::text(frame, controlAreaX, currentY, "Hue Range:", 0.35);
    currentY += 20;
    // 保存之前的值用于变化检测
    static int prev_hue_min = hue_min, prev_hue_max = hue_max;
    static int prev_sat_min = sat_min, prev_sat_max = sat_max;
    static int prev_val_min = val_min, prev_val_max = val_max;

    cvui::trackbar(frame, controlAreaX, currentY, 180, &hue_min, 0, 179);
    cvui::text(frame, controlAreaX + 190, currentY + 8, ("Min: " + std::to_string(hue_min)).c_str(), 0.3);
    currentY += 30;
    cvui::trackbar(frame, controlAreaX, currentY, 180, &hue_max, 0, 179);
    cvui::text(frame, controlAreaX + 190, currentY + 8, ("Max: " + std::to_string(hue_max)).c_str(), 0.3);
    currentY += 40;

    // Saturation range
    cvui::text(frame, controlAreaX, currentY, "Saturation Range:", 0.35);
    currentY += 20;
    cvui::trackbar(frame, controlAreaX, currentY, 180, &sat_min, 0, 255);
    cvui::text(frame, controlAreaX + 190, currentY + 8, ("Min: " + std::to_string(sat_min)).c_str(), 0.3);
    currentY += 30;
    cvui::trackbar(frame, controlAreaX, currentY, 180, &sat_max, 0, 255);
    cvui::text(frame, controlAreaX + 190, currentY + 8, ("Max: " + std::to_string(sat_max)).c_str(), 0.3);
    currentY += 40;

    // Value range
    cvui::text(frame, controlAreaX, currentY, "Value Range:", 0.35);
    currentY += 20;
    cvui::trackbar(frame, controlAreaX, currentY, 180, &val_min, 0, 255);
    cvui::text(frame, controlAreaX + 190, currentY + 8, ("Min: " + std::to_string(val_min)).c_str(), 0.3);
    currentY += 30;
    cvui::trackbar(frame, controlAreaX, currentY, 180, &val_max, 0, 255);
    cvui::text(frame, controlAreaX + 190, currentY + 8, ("Max: " + std::to_string(val_max)).c_str(), 0.3);
    currentY += 40;

    // 检测参数变化并自动更新预览
    if (hue_min != prev_hue_min || hue_max != prev_hue_max ||
        sat_min != prev_sat_min || sat_max != prev_sat_max ||
        val_min != prev_val_min || val_max != prev_val_max) {
        updateColorSelectPreview();
        prev_hue_min = hue_min; prev_hue_max = hue_max;
        prev_sat_min = sat_min; prev_sat_max = sat_max;
        prev_val_min = val_min; prev_val_max = val_max;
    }

    if (cvui::button(frame, controlAreaX, currentY, 120, 25, "Update Preview", 0.35)) {
        updateColorSelectPreview();
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderColorClusterModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Color Cluster", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 初始化预览图像（如果还没有预览图像）
    if (previewImage.empty() && processor.hasImage()) {
        previewImage = processor.getCurrentImage().clone();
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "K-Means Color Clustering:", 0.4);
    currentY += 30;

    cvui::text(frame, controlAreaX, currentY, "Number of Clusters (K):", 0.35);
    currentY += 20;

    // 保存之前的值用于变化检测
    static int prev_k_clusters = k_clusters;

    cvui::trackbar(frame, controlAreaX, currentY, 200, &k_clusters, 2, 16);
    cvui::text(frame, controlAreaX + 210, currentY + 8, ("K = " + std::to_string(k_clusters)).c_str(), 0.3);
    currentY += 40;

    // 检测参数变化并自动更新预览
    if (k_clusters != prev_k_clusters) {
        updateColorClusterPreview();
        prev_k_clusters = k_clusters;
    }

    cvui::text(frame, controlAreaX, currentY, "Current setting: " + std::to_string(k_clusters) + " clusters", 0.3);
    cvui::text(frame, controlAreaX, currentY + 15, "Higher values = more color detail", 0.25);
    cvui::text(frame, controlAreaX, currentY + 30, "Lower values = more color reduction", 0.25);

    if (cvui::button(frame, controlAreaX, currentY + 60, 120, 25, "Update Preview", 0.35)) {
        updateColorClusterPreview();
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderColorDeconvolutionModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Color Deconvolution", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "Color Channel Separation:", 0.4);
    currentY += 30;

    cvui::text(frame, controlAreaX, currentY, "Select Channel:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Blue Channel", 0.3)) {
        deconvolution_channel = 0;
        updateColorDeconvolutionPreview();
    }
    if (deconvolution_channel == 0) {
        cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Green Channel", 0.3)) {
        deconvolution_channel = 1;
        updateColorDeconvolutionPreview();
    }
    if (deconvolution_channel == 1) {
        cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Red Channel", 0.3)) {
        deconvolution_channel = 2;
        updateColorDeconvolutionPreview();
    }
    if (deconvolution_channel == 2) {
        cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
    }
    currentY += 40;

    const char* channelNames[] = {"Blue", "Green", "Red"};
    cvui::text(frame, controlAreaX, currentY, ("Current: " + std::string(channelNames[deconvolution_channel]) + " Channel").c_str(), 0.3);

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderChannelOperationModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Channel Operation", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    int currentY = controlAreaY;

    cvui::text(frame, controlAreaX, currentY, "Channel Arithmetic Operations:", 0.4);
    currentY += 30;

    cvui::text(frame, controlAreaX, currentY, "Operation Type:", 0.35);
    currentY += 25;

    const char* operations[] = {"Add", "Subtract", "Multiply", "Divide"};
    for (int i = 0; i < 4; i++) {
        if (cvui::button(frame, controlAreaX + (i % 2) * 110, currentY + (i / 2) * 30, 100, 25, operations[i], 0.3)) {
            operation_type = i;
            updateChannelOperationPreview();
        }
        if (operation_type == i) {
            cvui::text(frame, controlAreaX + (i % 2) * 110 + 105, currentY + (i / 2) * 30 + 8, "<-", 0.25);
        }
    }
    currentY += 70;

    cvui::text(frame, controlAreaX, currentY, "Operation Value:", 0.35);
    currentY += 20;
    cvui::trackbar(frame, controlAreaX, currentY, 200, &operation_value, 0.1, 5.0);
    cvui::text(frame, controlAreaX + 210, currentY + 8, ("Value: " + std::to_string(operation_value).substr(0, 4)).c_str(), 0.3);
    currentY += 40;

    cvui::text(frame, controlAreaX, currentY, ("Current: " + std::string(operations[operation_type]) + " " + std::to_string(operation_value).substr(0, 4)).c_str(), 0.3);

    if (cvui::button(frame, controlAreaX, currentY + 30, 120, 25, "Update Preview", 0.35)) {
        updateChannelOperationPreview();
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderSegmentationModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Segmentation Functions", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 初始化预览图像（如果还没有预览图像）
    if (previewImage.empty() && processor.hasImage()) {
        previewImage = processor.getCurrentImage().clone();
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    if (currentSegmentationFunction == SegmentationFunction::NONE) {
        // 功能选择界面
        SegmentationFunction selectedFunction = UIComponents::renderSegmentationFunctionSelection(frame, controlAreaX, controlAreaY);
        if (selectedFunction != SegmentationFunction::NONE) {
            currentSegmentationFunction = selectedFunction;

            // 重置参数到默认值
            switch (selectedFunction) {
                case SegmentationFunction::BASIC_THRESHOLD:
                    thresholdValue = 127.0;
                    thresholdType = 0;
                    break;
                case SegmentationFunction::RANGE_THRESHOLD:
                    thresholdMin = 50.0;
                    thresholdMax = 200.0;
                    break;
                case SegmentationFunction::ADAPTIVE_THRESHOLD:
                    adaptiveMethod = 0;
                    thresholdType = 0;
                    blockSize = 11;
                    C = 2.0;
                    break;
                default:
                    break;
            }

            updateSegmentationPreview(currentSegmentationFunction);
        }
    } else {
        // 参数控制界面
        int result = UIComponents::renderSegmentationParameters(frame, controlAreaX, controlAreaY, currentSegmentationFunction,
                                                              thresholdValue, thresholdType, thresholdMin, thresholdMax,
                                                              adaptiveMethod, blockSize, C);

        if (result == 1) {
            // Back button clicked
            currentSegmentationFunction = SegmentationFunction::NONE;
        } else if (result == 2) {
            // Update preview
            updateSegmentationPreview(currentSegmentationFunction);
        }
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderMorphologyModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Morphology Functions", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 初始化预览图像（如果还没有预览图像）
    if (previewImage.empty() && processor.hasImage()) {
        previewImage = processor.getCurrentImage().clone();
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    if (currentMorphologyFunction == MorphologyFunction::NONE) {
        // 功能选择界面
        MorphologyFunction selectedFunction = UIComponents::renderMorphologyFunctionSelection(frame, controlAreaX, controlAreaY);
        if (selectedFunction != MorphologyFunction::NONE) {
            currentMorphologyFunction = selectedFunction;

            // 重置参数到默认值
            morphKernelSize = 5;
            morphKernelType = 1; // ELLIPSE
            edgeThreshold = 100.0;
            separationMethod = 0; // Canny

            updateMorphologyPreview(currentMorphologyFunction);
        }
    } else {
        // 参数控制界面
        int result = UIComponents::renderMorphologyParameters(frame, controlAreaX, controlAreaY, currentMorphologyFunction,
                                                            morphKernelSize, morphKernelType, edgeThreshold, separationMethod);

        if (result == 1) {
            // Back button clicked
            currentMorphologyFunction = MorphologyFunction::NONE;
        } else if (result == 2) {
            // Update preview
            updateMorphologyPreview(currentMorphologyFunction);
        }
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderCleanUpModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Clean-Up Functions", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 初始化预览图像（如果还没有预览图像）
    if (previewImage.empty() && processor.hasImage()) {
        previewImage = processor.getCurrentImage().clone();
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    if (currentCleanUpFunction == CleanUpFunction::NONE) {
        // 功能选择界面
        CleanUpFunction selectedFunction = UIComponents::renderCleanUpFunctionSelection(frame, controlAreaX, controlAreaY);
        if (selectedFunction != CleanUpFunction::NONE) {
            currentCleanUpFunction = selectedFunction;

            // 重置参数到默认值
            switch (selectedFunction) {
                case CleanUpFunction::FILL_ALL_HOLES:
                    minHoleSize = 50;
                    fillMethod = 0;
                    break;
                case CleanUpFunction::REJECT_FEATURES:
                    minFeatureSize = 10;
                    maxFeatureSize = 1000;
                    rejectMethod = 0;
                    break;
                default:
                    break;
            }

            updateCleanUpPreview(currentCleanUpFunction);
        }
    } else {
        // 参数控制界面
        int result = UIComponents::renderCleanUpParameters(frame, controlAreaX, controlAreaY, currentCleanUpFunction,
                                                         minHoleSize, fillMethod, minFeatureSize, maxFeatureSize, rejectMethod);

        if (result == 1) {
            // Back button clicked
            currentCleanUpFunction = CleanUpFunction::NONE;
        } else if (result == 2) {
            // Update preview
            updateCleanUpPreview(currentCleanUpFunction);
        }
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

void ImageProcessingApp::renderMeasurementsModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Measurements Functions", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 初始化预览图像（如果还没有预览图像）
    if (previewImage.empty() && processor.hasImage()) {
        previewImage = processor.getCurrentImage().clone();
    }

    // 预览区域
    UIComponents::renderPreviewArea(frame, modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight, previewImage);

    // 控制区域
    if (currentMeasurementsFunction == MeasurementsFunction::NONE) {
        // 功能选择界面
        MeasurementsFunction selectedFunction = UIComponents::renderMeasurementsFunctionSelection(frame, controlAreaX, controlAreaY);
        if (selectedFunction != MeasurementsFunction::NONE) {
            currentMeasurementsFunction = selectedFunction;

            // 重置参数到默认值
            switch (selectedFunction) {
                case MeasurementsFunction::COUNT:
                    minObjectSize = 10;
                    maxObjectSize = 10000;
                    sensitivity = 0.5;
                    break;
                default:
                    break;
            }

            updateMeasurementsPreview(currentMeasurementsFunction);
        }
    } else {
        // 参数控制界面
        int result = UIComponents::renderMeasurementsParameters(frame, controlAreaX, controlAreaY, currentMeasurementsFunction,
                                                              minObjectSize, maxObjectSize, sensitivity);

        if (result == 1) {
            // Back button clicked
            currentMeasurementsFunction = MeasurementsFunction::NONE;
        } else if (result == 2) {
            // Update preview
            updateMeasurementsPreview(currentMeasurementsFunction);
        }
    }

    // 测量结果显示区域
    if (currentMeasurementsFunction != MeasurementsFunction::NONE) {
        int resultsAreaX = modalWindowX + 20;
        int resultsAreaY = modalWindowY + previewAreaHeight + 60;
        int resultsAreaWidth = previewAreaWidth;
        int resultsAreaHeight = 120;

        UIComponents::renderMeasurementResults(frame, resultsAreaX, resultsAreaY, resultsAreaWidth, resultsAreaHeight, lastMeasurementResult);
    }

    // Apply/Cancel按钮
    renderModalButtons(controlAreaX, modalWindowY + modalWindowHeight - 70);
}

// Measurements methods
void ImageProcessingApp::applyMeasurementsFunction(MeasurementsFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    try {
        cv::Mat currentImage = processor.getCurrentImage();

        std::vector<double> params = {(double)minObjectSize, (double)maxObjectSize, sensitivity};
        lastMeasurementResult = Measurements::applyFunction(currentImage, function, params);

        std::cout << "Applied measurements function: " << (int)function
                  << ", found " << lastMeasurementResult.objectCount << " objects" << std::endl;
        std::cout << "Total area: " << lastMeasurementResult.totalArea << " px²" << std::endl;
        std::cout << "Average size: " << lastMeasurementResult.averageSize << " px²" << std::endl;

        // Create visualization image and set it as current image
        cv::Mat visualizationImage = Measurements::createVisualizationImage(currentImage, lastMeasurementResult, minObjectSize, maxObjectSize);
        if (!visualizationImage.empty()) {
            processor.setCurrentImage(visualizationImage);
        }

    } catch (const std::exception& e) {
        std::cout << "Error applying measurements function: " << e.what() << std::endl;
    }
}

void ImageProcessingApp::updateMeasurementsPreview(MeasurementsFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();

    try {
        std::vector<double> params = {(double)minObjectSize, (double)maxObjectSize, sensitivity};
        MeasurementResult result = Measurements::applyFunction(tempImage, function, params);

        // Create visualization for preview
        cv::Mat visualizationImage = Measurements::createVisualizationImage(tempImage, result, minObjectSize, maxObjectSize);

        if (!visualizationImage.empty()) {
            previewImage = visualizationImage.clone();
            lastMeasurementResult = result; // Store for display
        }

    } catch (const std::exception& e) {
        std::cout << "Error updating measurements preview: " << e.what() << std::endl;
    }
}
