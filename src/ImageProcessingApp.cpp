#define CVUI_IMPLEMENTATION
#include "ImageProcessingApp.h"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

ImageProcessingApp::ImageProcessingApp() : windowName("Image Processing Application") {
    initializeUI();
}

ImageProcessingApp::~ImageProcessingApp() {
}

void ImageProcessingApp::initialize() {
    // 初始化cvui
    cvui::init(windowName);

    std::cout << "Image Processing Application Started" << std::endl;
    std::cout << "Supported Features:" << std::endl;
    std::cout << "1. Image Loading and Display" << std::endl;
    std::cout << "2. Color Image Processing" << std::endl;
}

void ImageProcessingApp::initializeUI() {
    windowWidth = 1000;
    windowHeight = 680;

    // 图像显示区域
    imageDisplayX = 15;
    imageDisplayY = 15;
    imageDisplayWidth = windowWidth - 30;
    imageDisplayHeight = 400;

    // 初始化颜色选择参数
    hue_min = 0; hue_max = 180;
    sat_min = 0; sat_max = 255;
    val_min = 0; val_max = 255;

    // 初始化K-means参数
    k_clusters = 3;

    // 初始化通道选择
    selected_channel = 0;

    // 初始化通道操作参数
    operation_type = 0;
    operation_value = 1.0;

    // UI控制变量
    selectedImagePath = "";

    // 统一模态窗口系统
    currentModal = ModalFunction::NONE;
    isProcessing = false;

    // 模态窗口布局
    modalWindowX = 150;
    modalWindowY = 100;
    modalWindowWidth = 700;
    modalWindowHeight = 450;
    previewAreaWidth = 300;
    previewAreaHeight = 250;
    controlAreaX = modalWindowX + previewAreaWidth + 30;
    controlAreaY = modalWindowY + 40;

    // 预处理功能初始化
    currentPreProcessingFunction = PreProcessingFunction::NONE;

    // 预处理参数初始化
    brightness = 0.0;              // -100 to +100
    contrast = 1.0;                // 0.1 to 3.0
    kernelSize = 5;
    sigmaX = 1.0;
    sigmaY = 1.0;
    h = 10.0;
    templateWindowSize = 7;
    searchWindowSize = 21;
    sharpenStrength = 1.0;

    // CONTRAST类别参数初始化
    histogramMethod = 0;           // 0=global, 1=adaptive
    clipLimit = 2.0;               // 1.0 to 40.0
    flattenKernelSize = 15;        // 5 to 51, odd only

    // 初始化前一个值变量
    prevBrightness = brightness;
    prevContrast = contrast;
    prevClipLimit = clipLimit;
    prevFlattenKernelSize = flattenKernelSize;

    // 创建主界面Mat
    frame = cv::Mat::zeros(windowHeight, windowWidth, CV_8UC3);
}

void ImageProcessingApp::run() {
    while (true) {
        // 清空画布
        frame = cv::Scalar(49, 52, 49);

        // 渲染UI
        renderUI();

        // 更新cvui
        cvui::update();

        // 显示界面
        cv::imshow(windowName, frame);

        // 检查按键
        int key = cv::waitKey(30) & 0xFF;
        if (key == 27) { // ESC键退出
            break;
        }
    }

    cv::destroyAllWindows();
}

void ImageProcessingApp::renderUI() {
    // 渲染主控制面板
    renderMainControlPanel();

    // 渲染图像显示区域
    renderImageDisplay();

    // 渲染模态窗口（如果需要显示）
    if (currentModal != ModalFunction::NONE) {
        renderModalWindow();
    }
}

void ImageProcessingApp::renderMainControlPanel() {
    // 主控制面板 - 位于图像显示区域下方
    int panelY = imageDisplayY + imageDisplayHeight + 20;
    int panelHeight = 140;

    cvui::window(frame, 15, panelY, windowWidth - 30, panelHeight, "Image Processing Functions", 0.4);

    // 计算按钮布局
    int buttonWidth = 120;
    int buttonHeight = 30;
    int buttonSpacing = 10;
    int startX = 30;
    int row1Y = panelY + 30;
    int row2Y = panelY + 70;

    // 第一行：基础功能
    if (cvui::button(frame, startX, row1Y, buttonWidth, buttonHeight, "Load Image", 0.35)) {
        openModal(ModalFunction::LOAD_IMAGE);
    }

    if (cvui::button(frame, startX + (buttonWidth + buttonSpacing), row1Y, buttonWidth, buttonHeight, "Reset Image", 0.35)) {
        openModal(ModalFunction::RESET_IMAGE);
    }

    if (cvui::button(frame, startX + 2 * (buttonWidth + buttonSpacing), row1Y, buttonWidth, buttonHeight, "Grayscale", 0.35)) {
        openModal(ModalFunction::CONVERT_GRAYSCALE);
    }

    // 第二行：高级功能
    if (cvui::button(frame, startX, row2Y, buttonWidth, buttonHeight, "Color Select", 0.35)) {
        openModal(ModalFunction::COLOR_SELECT);
    }

    if (cvui::button(frame, startX + (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight, "Clustering", 0.35)) {
        openModal(ModalFunction::COLOR_CLUSTER);
    }

    if (cvui::button(frame, startX + 2 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight, "Deconvolution", 0.35)) {
        openModal(ModalFunction::COLOR_DECONVOLUTION);
    }

    if (cvui::button(frame, startX + 3 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight, "Channel Ops", 0.35)) {
        openModal(ModalFunction::CHANNEL_OPERATION);
    }

    if (cvui::button(frame, startX + 4 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight, "Pre-Processing", 0.35)) {
        openModal(ModalFunction::PRE_PROCESSING);
    }

    // 显示当前图像信息
    int infoX = startX + 5 * (buttonWidth + buttonSpacing);
    if (!selectedImagePath.empty()) {
        std::string filename = selectedImagePath.substr(selectedImagePath.find_last_of("/\\") + 1);
        if (filename.length() > 15) filename = filename.substr(0, 12) + "...";
        cvui::text(frame, infoX, row1Y + 5, "Current: " + filename, 0.3);
    }

    if (processor.hasImage()) {
        cv::Size size = processor.getImageSize();
        cvui::text(frame, infoX, row1Y + 20, "Size: " + std::to_string(size.width) + "x" + std::to_string(size.height), 0.3);
    }

    // 显示处理状态
    if (isProcessing) {
        cvui::text(frame, infoX, row2Y + 5, "Processing...", 0.3);
    }
}

void ImageProcessingApp::renderImageDisplay() {
    // 图像显示区域
    cvui::window(frame, imageDisplayX, imageDisplayY, imageDisplayWidth, imageDisplayHeight + 20, "Image Display", 0.4);

    if (processor.hasImage()) {
        cv::Mat displayImg = scaleImageToFit(processor.getDisplayImage());
        if (!displayImg.empty()) {
            // 计算居中位置
            int imgX = imageDisplayX + (imageDisplayWidth - displayImg.cols) / 2;
            int imgY = imageDisplayY + 30 + (imageDisplayHeight - displayImg.rows) / 2;

            // 使用cvui::image来显示图像
            // 强制创建新的图像副本以避免缓存问题
            cv::Mat freshImage = displayImg.clone();
            cvui::image(frame, imgX, imgY, freshImage);
        }
    } else {
        // 显示提示信息
        cvui::text(frame, imageDisplayX + imageDisplayWidth/2 - 60,
                  imageDisplayY + imageDisplayHeight/2, "No image loaded", 0.5);
        cvui::text(frame, imageDisplayX + imageDisplayWidth/2 - 80,
                  imageDisplayY + imageDisplayHeight/2 + 25, "Click 'Load Image' to start", 0.35);
    }
}

void ImageProcessingApp::openModal(ModalFunction function) {
    currentModal = function;
    if (function != ModalFunction::LOAD_IMAGE && function != ModalFunction::RESET_IMAGE) {
        updatePreview();
    }
}

void ImageProcessingApp::closeModal() {
    // 重置预处理功能选择
    currentPreProcessingFunction = PreProcessingFunction::NONE;

    currentModal = ModalFunction::NONE;
    previewImage = cv::Mat();
    isProcessing = false;
}

void ImageProcessingApp::renderModalWindow() {
    switch (currentModal) {
        case ModalFunction::LOAD_IMAGE:
            renderLoadImageModal();
            break;
        case ModalFunction::CONVERT_GRAYSCALE:
            renderGrayscaleModal();
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
        default:
            break;
    }
}

void ImageProcessingApp::renderLoadImageModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, 200, "Load Image", 0.4);

    cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Select an image file to load:", 0.4);

    if (cvui::button(frame, modalWindowX + 20, modalWindowY + 70, 120, 30, "Browse Files", 0.35)) {
        std::string filepath = openFileDialog();
        if (!filepath.empty()) {
            if (processor.loadImage(filepath)) {
                selectedImagePath = filepath;
                std::cout << "Successfully loaded image: " << filepath << std::endl;
                closeModal();
            } else {
                std::cout << "Failed to load image: " << filepath << std::endl;
            }
        }
    }

    if (cvui::button(frame, modalWindowX + 160, modalWindowY + 70, 80, 30, "Cancel", 0.35)) {
        closeModal();
    }

    // 显示当前选择的文件
    if (!selectedImagePath.empty()) {
        std::string filename = selectedImagePath.substr(selectedImagePath.find_last_of("/\\") + 1);
        cvui::text(frame, modalWindowX + 20, modalWindowY + 120, "Current: " + filename, 0.3);
    }
}

void ImageProcessingApp::renderResetImageModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, 200, "Reset Image", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "No image loaded to reset.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Reset image to original state?", 0.4);
    cvui::text(frame, modalWindowX + 20, modalWindowY + 65, "This will undo all processing operations.", 0.35);

    if (cvui::button(frame, modalWindowX + 20, modalWindowY + 100, 80, 30, "Reset", 0.35)) {
        processor.resetToOriginal();
        std::cout << "Reset to original image" << std::endl;
        closeModal();
    }

    if (cvui::button(frame, modalWindowX + 120, modalWindowY + 100, 80, 30, "Cancel", 0.35)) {
        closeModal();
    }
}

void ImageProcessingApp::renderGrayscaleModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Convert to Grayscale", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 预览区域
    renderPreviewArea(modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight);

    // 控制区域
    cvui::text(frame, controlAreaX, controlAreaY, "Convert to Grayscale", 0.4);
    cvui::text(frame, controlAreaX, controlAreaY + 30, "This will convert the image to", 0.35);
    cvui::text(frame, controlAreaX, controlAreaY + 50, "grayscale (black and white).", 0.35);
    cvui::text(frame, controlAreaX, controlAreaY + 80, "Preview shows the result.", 0.35);

    // 按钮区域
    renderModalButtons(modalWindowX + 20, modalWindowY + modalWindowHeight - 60);
}

void ImageProcessingApp::renderColorSelectModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Color Select (HSV)", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 预览区域
    renderPreviewArea(modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight);

    // 控制区域
    int currentY = controlAreaY;
    cvui::text(frame, controlAreaX, currentY, "HSV Color Range Selection:", 0.4);
    currentY += 30;

    cvui::text(frame, controlAreaX, currentY, "Hue Range:", 0.35);
    cvui::trackbar(frame, controlAreaX + 80, currentY - 5, 150, &hue_min, 0, 180);
    currentY += 25;
    cvui::trackbar(frame, controlAreaX + 80, currentY - 5, 150, &hue_max, 0, 180);
    currentY += 35;

    cvui::text(frame, controlAreaX, currentY, "Saturation:", 0.35);
    cvui::trackbar(frame, controlAreaX + 80, currentY - 5, 150, &sat_min, 0, 255);
    currentY += 25;
    cvui::trackbar(frame, controlAreaX + 80, currentY - 5, 150, &sat_max, 0, 255);
    currentY += 35;

    cvui::text(frame, controlAreaX, currentY, "Value:", 0.35);
    cvui::trackbar(frame, controlAreaX + 80, currentY - 5, 150, &val_min, 0, 255);
    currentY += 25;
    cvui::trackbar(frame, controlAreaX + 80, currentY - 5, 150, &val_max, 0, 255);
    currentY += 35;

    // 显示当前值
    cvui::text(frame, controlAreaX, currentY, "H: " + std::to_string(hue_min) + "-" + std::to_string(hue_max), 0.3);
    cvui::text(frame, controlAreaX, currentY + 15, "S: " + std::to_string(sat_min) + "-" + std::to_string(sat_max), 0.3);
    cvui::text(frame, controlAreaX, currentY + 30, "V: " + std::to_string(val_min) + "-" + std::to_string(val_max), 0.3);

    // 实时更新预览按钮
    if (cvui::button(frame, controlAreaX, currentY + 60, 120, 25, "Update Preview", 0.35)) {
        updatePreview();
    }

    // 按钮区域
    renderModalButtons(modalWindowX + 20, modalWindowY + modalWindowHeight - 60);
}

void ImageProcessingApp::renderColorClusterModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Color Clustering (K-Means)", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 预览区域
    renderPreviewArea(modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight);

    // 控制区域
    int currentY = controlAreaY;
    cvui::text(frame, controlAreaX, currentY, "K-Means Clustering:", 0.4);
    currentY += 30;

    cvui::text(frame, controlAreaX, currentY, "Number of clusters (K):", 0.35);
    currentY += 25;
    cvui::trackbar(frame, controlAreaX, currentY - 5, 200, &k_clusters, 2, 8);
    currentY += 35;

    cvui::text(frame, controlAreaX, currentY, "Current K: " + std::to_string(k_clusters), 0.35);
    currentY += 30;

    cvui::text(frame, controlAreaX, currentY, "This will group similar colors", 0.3);
    cvui::text(frame, controlAreaX, currentY + 15, "together to reduce the color", 0.3);
    cvui::text(frame, controlAreaX, currentY + 30, "palette of the image.", 0.3);

    // 实时更新预览按钮
    if (cvui::button(frame, controlAreaX, currentY + 60, 120, 25, "Update Preview", 0.35)) {
        updatePreview();
    }

    // 按钮区域
    renderModalButtons(modalWindowX + 20, modalWindowY + modalWindowHeight - 60);
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
    renderPreviewArea(modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight);

    // 控制区域
    int currentY = controlAreaY;
    cvui::text(frame, controlAreaX, currentY, "Channel Selection:", 0.4);
    currentY += 30;

    const char* channels[] = {"Blue Channel", "Green Channel", "Red Channel"};
    for (int i = 0; i < 3; i++) {
        if (cvui::button(frame, controlAreaX, currentY + i * 35, 120, 30, channels[i], 0.35)) {
            selected_channel = i;
            updatePreview();
        }
        if (selected_channel == i) {
            cvui::text(frame, controlAreaX + 130, currentY + i * 35 + 8, "<- Selected", 0.3);
        }
    }

    currentY += 120;
    cvui::text(frame, controlAreaX, currentY, "Extract a single color channel", 0.3);
    cvui::text(frame, controlAreaX, currentY + 15, "from the RGB image.", 0.3);

    // 按钮区域
    renderModalButtons(modalWindowX + 20, modalWindowY + modalWindowHeight - 60);
}

void ImageProcessingApp::renderChannelOperationModal() {
    cvui::window(frame, modalWindowX, modalWindowY, modalWindowWidth, modalWindowHeight, "Channel Operations", 0.4);

    if (!processor.hasImage()) {
        cvui::text(frame, modalWindowX + 20, modalWindowY + 40, "Please load an image first.", 0.4);
        if (cvui::button(frame, modalWindowX + 20, modalWindowY + 80, 80, 30, "Close", 0.35)) {
            closeModal();
        }
        return;
    }

    // 预览区域
    renderPreviewArea(modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight);

    // 控制区域
    int currentY = controlAreaY;
    cvui::text(frame, controlAreaX, currentY, "Channel Operation:", 0.4);
    currentY += 30;

    const char* operations[] = {"Add", "Subtract", "Multiply", "Divide"};
    cvui::text(frame, controlAreaX, currentY, "Operation Type:", 0.35);
    currentY += 25;

    for (int i = 0; i < 4; i++) {
        if (cvui::button(frame, controlAreaX + (i % 2) * 110, currentY + (i / 2) * 35, 100, 30, operations[i], 0.35)) {
            operation_type = i;
            updatePreview();
        }
        if (operation_type == i) {
            cvui::text(frame, controlAreaX + (i % 2) * 110 + 105, currentY + (i / 2) * 35 + 8, "<-", 0.3);
        }
    }
    currentY += 80;

    cvui::text(frame, controlAreaX, currentY, "Value:", 0.35);
    currentY += 25;
    cvui::trackbar(frame, controlAreaX, currentY - 5, 200, &operation_value, 0.1, 5.0);
    currentY += 35;

    std::string currentOp = std::string("Current: ") + operations[operation_type] + " " + std::to_string(operation_value);
    cvui::text(frame, controlAreaX, currentY, currentOp, 0.3);

    // 实时更新预览按钮
    if (cvui::button(frame, controlAreaX, currentY + 30, 120, 25, "Update Preview", 0.35)) {
        updatePreview();
    }

    // 按钮区域
    renderModalButtons(modalWindowX + 20, modalWindowY + modalWindowHeight - 60);
}

void ImageProcessingApp::renderPreviewArea(int x, int y, int width, int height) {
    // 预览区域背景
    cvui::rect(frame, x, y, width, height, 0x333333, 0x666666);
    cvui::text(frame, x + 5, y - 15, "Preview:", 0.35);

    if (!previewImage.empty()) {
        cv::Mat scaledPreview;
        double scale = std::min((double)(width - 10) / previewImage.cols,
                               (double)(height - 10) / previewImage.rows);
        if (scale < 1.0) {
            cv::resize(previewImage, scaledPreview, cv::Size(0, 0), scale, scale, cv::INTER_AREA);
        } else {
            scaledPreview = previewImage.clone();
        }

        int imgX = x + (width - scaledPreview.cols) / 2;
        int imgY = y + (height - scaledPreview.rows) / 2;
        cvui::image(frame, imgX, imgY, scaledPreview);
    } else if (processor.hasImage()) {
        cvui::text(frame, x + width/2 - 40, y + height/2, "No preview", 0.35);
    } else {
        cvui::text(frame, x + width/2 - 50, y + height/2, "No image loaded", 0.35);
    }
}

void ImageProcessingApp::renderModalButtons(int x, int y) {
    if (cvui::button(frame, x, y, 80, 30, "Apply", 0.35)) {
        std::cout << "DEBUG: Apply button clicked!" << std::endl;
        applyCurrentFunction();
        std::cout << "DEBUG: applyCurrentFunction() completed, closing modal" << std::endl;
        closeModal();
        std::cout << "DEBUG: Modal closed" << std::endl;
    }

    if (cvui::button(frame, x + 100, y, 80, 30, "Cancel", 0.35)) {
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

    // 预览区域
    renderPreviewArea(modalWindowX + 20, modalWindowY + 40, previewAreaWidth, previewAreaHeight);

    // 控制区域
    int currentY = controlAreaY;

    // 如果没有选择功能，显示功能选择界面
    if (currentPreProcessingFunction == PreProcessingFunction::NONE) {
        cvui::text(frame, controlAreaX, currentY, "Select a Pre-Processing Function:", 0.4);
        currentY += 30;

        // CONTRAST (对比度调节)
        cvui::text(frame, controlAreaX, currentY, "CONTRAST:", 0.35);
        currentY += 25;

        if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Adjust Contrast", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::ADJUST_CONTRAST;
            // 重置Adjust Contrast参数到默认值
            brightness = 0.0;
            contrast = 1.0;
            // 重置前一个值以确保首次预览更新
            prevBrightness = brightness - 1; // 强制不同以触发更新
            prevContrast = contrast - 1;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Histogram Eq", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::HISTOGRAM_EQUALIZATION;
            // 重置Histogram Equalization参数到默认值
            histogramMethod = 0;  // 0=global
            clipLimit = 2.0;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        currentY += 30;

        if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Flatten BG", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::FLATTEN_BACKGROUND;
            // 重置Flatten Background参数到默认值
            flattenKernelSize = 15;
            // 重置前一个值以确保首次预览更新
            prevFlattenKernelSize = flattenKernelSize - 2; // 强制不同以触发更新
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        currentY += 40;

        // NOISE-REDUCTION (降噪处理)
        cvui::text(frame, controlAreaX, currentY, "NOISE-REDUCTION:", 0.35);
        currentY += 25;

        if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Median Filter", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::MEDIAN_FILTER;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Wiener Filter", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::WIENER_FILTER;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        currentY += 30;

        if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Non-Local Means", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::NON_LOCAL_MEANS;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        currentY += 40;

        // BLUR (模糊处理)
        cvui::text(frame, controlAreaX, currentY, "BLUR:", 0.35);
        currentY += 25;

        if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Gaussian Blur", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::GAUSSIAN_BLUR;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "Average Blur", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::AVERAGE_BLUR;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        currentY += 30;

        if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Sum Filter", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::SUM_FILTER;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        if (cvui::button(frame, controlAreaX + 110, currentY, 100, 25, "GS Dilate", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::GRAYSCALE_DILATE;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
        currentY += 30;

        if (cvui::button(frame, controlAreaX, currentY, 100, 25, "GS Erode", 0.3)) {
            currentPreProcessingFunction = PreProcessingFunction::GRAYSCALE_ERODE;
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
    } else {
        // 显示选中功能的参数控制
        renderPreProcessingParameters();
    }

    // 按钮区域
    renderModalButtons(modalWindowX + 20, modalWindowY + modalWindowHeight - 60);
}

void ImageProcessingApp::renderPreProcessingParameters() {
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

    cvui::text(frame, controlAreaX, currentY, functionNames[(int)currentPreProcessingFunction], 0.4);

    if (cvui::button(frame, controlAreaX + 200, currentY - 5, 80, 25, "< Back", 0.3)) {
        currentPreProcessingFunction = PreProcessingFunction::NONE;
        return;
    }
    currentY += 35;

    // 根据选择的功能显示相应的参数控制
    switch (currentPreProcessingFunction) {
        case PreProcessingFunction::ADJUST_CONTRAST:
            renderAdjustContrastParameters(currentY);
            break;
        case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
            renderHistogramEqualizationParameters(currentY);
            break;
        case PreProcessingFunction::FLATTEN_BACKGROUND:
            renderFlattenBackgroundParameters(currentY);
            break;
        default:
            cvui::text(frame, controlAreaX, currentY, "No parameters for this function.", 0.35);
            cvui::text(frame, controlAreaX, currentY + 25, "Click Apply to execute.", 0.35);
            break;
    }
}

void ImageProcessingApp::renderAdjustContrastParameters(int startY) {
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
    if (brightness != prevBrightness || contrast != prevContrast) {
        updatePreProcessingPreview(currentPreProcessingFunction);
        prevBrightness = brightness;
        prevContrast = contrast;
    }

    if (cvui::button(frame, controlAreaX, currentY + 40, 120, 25, "Update Preview", 0.35)) {
        updatePreProcessingPreview(currentPreProcessingFunction);
    }
}

void ImageProcessingApp::renderHistogramEqualizationParameters(int startY) {
    int currentY = startY;

    cvui::text(frame, controlAreaX, currentY, "Equalization Method:", 0.35);
    currentY += 25;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Global", 0.3)) {
        histogramMethod = 0;
        updatePreProcessingPreview(currentPreProcessingFunction);
    }
    if (histogramMethod == 0) {
        cvui::text(frame, controlAreaX + 110, currentY + 8, "<- Selected", 0.25);
    }
    currentY += 30;

    if (cvui::button(frame, controlAreaX, currentY, 100, 25, "Adaptive (CLAHE)", 0.3)) {
        histogramMethod = 1;
        updatePreProcessingPreview(currentPreProcessingFunction);
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
            updatePreProcessingPreview(currentPreProcessingFunction);
            prevClipLimit = clipLimit;
        }

        if (cvui::button(frame, controlAreaX, currentY, 120, 25, "Update Preview", 0.35)) {
            updatePreProcessingPreview(currentPreProcessingFunction);
        }
    }
}

void ImageProcessingApp::renderFlattenBackgroundParameters(int startY) {
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
    if (flattenKernelSize != prevFlattenKernelSize) {
        updatePreProcessingPreview(currentPreProcessingFunction);
        prevFlattenKernelSize = flattenKernelSize;
    }

    if (cvui::button(frame, controlAreaX, currentY + 40, 120, 25, "Update Preview", 0.35)) {
        updatePreProcessingPreview(currentPreProcessingFunction);
    }
}

std::string ImageProcessingApp::openFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};

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

    if (GetOpenFileNameA(&ofn)) {
        return std::string(szFile);
    } else {
        // 用户取消了对话框
        std::cout << "File dialog cancelled by user" << std::endl;
        return "";
    }
#else
    // 非Windows平台的备用方案
    std::cout << "Please enter image file path: ";
    std::string path;
    std::getline(std::cin, path);
    return path;
#endif
}

cv::Mat ImageProcessingApp::scaleImageToFit(const cv::Mat& image) {
    if (image.empty()) {
        return cv::Mat();
    }

    int maxWidth = imageDisplayWidth - 40;  // 留出边距
    int maxHeight = imageDisplayHeight - 40;

    double scaleX = (double)maxWidth / image.cols;
    double scaleY = (double)maxHeight / image.rows;
    double scale = std::min(scaleX, scaleY);

    if (scale >= 1.0) {
        return image.clone();
    }

    cv::Mat scaled;
    cv::resize(image, scaled, cv::Size(0, 0), scale, scale, cv::INTER_AREA);
    return scaled;
}

void ImageProcessingApp::updatePreview() {
    if (!processor.hasImage()) {
        return;
    }

    // 创建当前图像的副本用于预览
    cv::Mat tempImage = processor.getCurrentImage().clone();

    try {
        switch (currentModal) {
            case ModalFunction::CONVERT_GRAYSCALE:
                if (tempImage.channels() == 3) {
                    cv::Mat grayImage;
                    cv::cvtColor(tempImage, grayImage, cv::COLOR_BGR2GRAY);
                    cv::cvtColor(grayImage, tempImage, cv::COLOR_GRAY2BGR);
                }
                break;

            case ModalFunction::COLOR_SELECT:
                {
                    cv::Mat hsvImage;
                    cv::cvtColor(tempImage, hsvImage, cv::COLOR_BGR2HSV);
                    cv::Mat mask;
                    cv::Scalar lower(hue_min, sat_min, val_min);
                    cv::Scalar upper(hue_max, sat_max, val_max);
                    cv::inRange(hsvImage, lower, upper, mask);
                    cv::Mat result = cv::Mat::zeros(tempImage.size(), tempImage.type());
                    tempImage.copyTo(result, mask);
                    tempImage = result;
                }
                break;

            case ModalFunction::COLOR_CLUSTER:
                tempImage = performKMeans(tempImage, k_clusters);
                break;

            case ModalFunction::COLOR_DECONVOLUTION:
                {
                    std::vector<cv::Mat> channels;
                    cv::split(tempImage, channels);
                    cv::Mat singleChannel = channels[selected_channel];
                    cv::cvtColor(singleChannel, tempImage, cv::COLOR_GRAY2BGR);
                }
                break;

            case ModalFunction::CHANNEL_OPERATION:
                {
                    if (operation_type == 0) { // add
                        cv::add(tempImage, cv::Scalar::all(operation_value), tempImage);
                    } else if (operation_type == 1) { // subtract
                        cv::subtract(tempImage, cv::Scalar::all(operation_value), tempImage);
                    } else if (operation_type == 2) { // multiply
                        cv::multiply(tempImage, cv::Scalar::all(operation_value), tempImage);
                    } else if (operation_type == 3 && operation_value != 0) { // divide
                        cv::divide(tempImage, cv::Scalar::all(operation_value), tempImage);
                    }
                }
                break;

            case ModalFunction::PRE_PROCESSING:
                if (currentPreProcessingFunction != PreProcessingFunction::NONE) {
                    updatePreProcessingPreview(currentPreProcessingFunction);
                    return; // updatePreProcessingPreview已经设置了previewImage
                }
                break;

            default:
                break;
        }

        previewImage = tempImage;
    } catch (const std::exception& e) {
        std::cout << "Preview error: " << e.what() << std::endl;
    }
}

void ImageProcessingApp::applyCurrentFunction() {
    if (!processor.hasImage()) {
        return;
    }

    isProcessing = true;

    try {
        switch (currentModal) {
            case ModalFunction::CONVERT_GRAYSCALE:
                processor.convertToGrayscale();
                std::cout << "Converted to grayscale" << std::endl;
                break;

            case ModalFunction::COLOR_SELECT:
                processor.colorSelect(hue_min, hue_max, sat_min, sat_max, val_min, val_max);
                std::cout << "Color selection applied" << std::endl;
                break;

            case ModalFunction::COLOR_CLUSTER:
                processor.colorCluster(k_clusters);
                std::cout << "Color clustering applied with K=" << k_clusters << std::endl;
                break;

            case ModalFunction::COLOR_DECONVOLUTION:
                {
                    processor.colorDeconvolution(selected_channel);
                    std::string channelNames[] = {"Blue", "Green", "Red"};
                    std::cout << channelNames[selected_channel] << " channel extracted" << std::endl;
                }
                break;

            case ModalFunction::CHANNEL_OPERATION:
                {
                    std::string op_names[] = {"add", "subtract", "multiply", "divide"};
                    processor.channelOperation(op_names[operation_type], operation_value);
                    std::cout << "Channel operation applied: " << op_names[operation_type]
                              << " " << operation_value << std::endl;
                }
                break;

            case ModalFunction::PRE_PROCESSING:
                if (currentPreProcessingFunction != PreProcessingFunction::NONE) {
                    std::cout << "DEBUG: Applying pre-processing function..." << std::endl;
                    applyPreProcessingFunction(currentPreProcessingFunction);
                    std::cout << "DEBUG: Pre-processing function applied, forcing display update..." << std::endl;
                    // 强制更新显示图像
                    processor.updateDisplayImage();
                }
                break;

            default:
                break;
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    isProcessing = false;
}



cv::Mat ImageProcessingApp::performKMeans(const cv::Mat& image, int k) {
    // 准备数据
    cv::Mat data;
    image.convertTo(data, CV_32F);
    data = data.reshape(1, data.total());

    // K-Means聚类
    cv::Mat labels, centers;
    cv::kmeans(data, k, labels,
               cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 20, 1.0),
               3, cv::KMEANS_PP_CENTERS, centers);

    // 重构图像
    centers.convertTo(centers, CV_8U);
    cv::Mat result(image.size(), image.type());

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int cluster_idx = labels.at<int>(i * image.cols + j);
            if (image.channels() == 3) {
                result.at<cv::Vec3b>(i, j) = cv::Vec3b(
                    centers.at<uchar>(cluster_idx, 0),
                    centers.at<uchar>(cluster_idx, 1),
                    centers.at<uchar>(cluster_idx, 2)
                );
            } else {
                result.at<uchar>(i, j) = centers.at<uchar>(cluster_idx, 0);
            }
        }
    }

    return result;
}

void ImageProcessingApp::updatePreProcessingPreview(PreProcessingFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    cv::Mat tempImage = processor.getCurrentImage().clone();

    try {
        switch (function) {
            case PreProcessingFunction::ADJUST_CONTRAST:
                tempImage = applyAdjustContrast(tempImage, brightness, contrast);
                break;
            case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
                tempImage = applyHistogramEqualization(tempImage);
                break;
            case PreProcessingFunction::FLATTEN_BACKGROUND:
                tempImage = applyFlattenBackground(tempImage, flattenKernelSize);
                break;
            case PreProcessingFunction::MEDIAN_FILTER:
                tempImage = applyMedianFilter(tempImage, kernelSize);
                break;
            case PreProcessingFunction::WIENER_FILTER:
                tempImage = applyWienerFilter(tempImage, kernelSize);
                break;
            case PreProcessingFunction::NON_LOCAL_MEANS:
                tempImage = applyNonLocalMeans(tempImage, h, templateWindowSize, searchWindowSize);
                break;
            case PreProcessingFunction::GAUSSIAN_BLUR:
                tempImage = applyGaussianBlur(tempImage, kernelSize, sigmaX, sigmaY);
                break;
            case PreProcessingFunction::AVERAGE_BLUR:
                tempImage = applyAverageBlur(tempImage, kernelSize);
                break;
            case PreProcessingFunction::SUM_FILTER:
                tempImage = applySumFilter(tempImage, kernelSize);
                break;
            case PreProcessingFunction::GRAYSCALE_DILATE:
                tempImage = applyGrayscaleDilate(tempImage, kernelSize);
                break;
            case PreProcessingFunction::GRAYSCALE_ERODE:
                tempImage = applyGrayscaleErode(tempImage, kernelSize);
                break;
            default:
                break;
        }

        previewImage = tempImage;
    } catch (const std::exception& e) {
        std::cout << "Preview error: " << e.what() << std::endl;
    }
}

void ImageProcessingApp::applyPreProcessingFunction(PreProcessingFunction function) {
    if (!processor.hasImage()) {
        return;
    }

    try {
        cv::Mat result;
        cv::Mat currentImage = processor.getCurrentImage();

        switch (function) {
            case PreProcessingFunction::ADJUST_CONTRAST:
                result = applyAdjustContrast(currentImage, brightness, contrast);
                std::cout << "Applied contrast adjustment: brightness=" << brightness << ", contrast=" << contrast << std::endl;
                break;
            case PreProcessingFunction::HISTOGRAM_EQUALIZATION:
                result = applyHistogramEqualization(currentImage);
                std::cout << "Applied histogram equalization: method=" << (histogramMethod == 0 ? "global" : "adaptive")
                          << (histogramMethod == 1 ? ", clip limit=" + std::to_string(clipLimit) : "") << std::endl;
                break;
            case PreProcessingFunction::FLATTEN_BACKGROUND:
                result = applyFlattenBackground(currentImage, flattenKernelSize);
                std::cout << "Applied background flattening with kernel size=" << flattenKernelSize << std::endl;
                break;
            case PreProcessingFunction::MEDIAN_FILTER:
                result = applyMedianFilter(currentImage, kernelSize);
                std::cout << "Applied median filter with kernel size=" << kernelSize << std::endl;
                break;
            case PreProcessingFunction::WIENER_FILTER:
                result = applyWienerFilter(currentImage, kernelSize);
                std::cout << "Applied Wiener filter with kernel size=" << kernelSize << std::endl;
                break;
            case PreProcessingFunction::NON_LOCAL_MEANS:
                result = applyNonLocalMeans(currentImage, h, templateWindowSize, searchWindowSize);
                std::cout << "Applied non-local means denoising" << std::endl;
                break;
            case PreProcessingFunction::GAUSSIAN_BLUR:
                result = applyGaussianBlur(currentImage, kernelSize, sigmaX, sigmaY);
                std::cout << "Applied Gaussian blur with kernel size=" << kernelSize << std::endl;
                break;
            case PreProcessingFunction::AVERAGE_BLUR:
                result = applyAverageBlur(currentImage, kernelSize);
                std::cout << "Applied average blur with kernel size=" << kernelSize << std::endl;
                break;
            case PreProcessingFunction::SUM_FILTER:
                result = applySumFilter(currentImage, kernelSize);
                std::cout << "Applied sum filter with kernel size=" << kernelSize << std::endl;
                break;
            case PreProcessingFunction::GRAYSCALE_DILATE:
                result = applyGrayscaleDilate(currentImage, kernelSize);
                std::cout << "Applied grayscale dilation with kernel size=" << kernelSize << std::endl;
                break;
            case PreProcessingFunction::GRAYSCALE_ERODE:
                result = applyGrayscaleErode(currentImage, kernelSize);
                std::cout << "Applied grayscale erosion with kernel size=" << kernelSize << std::endl;
                break;
            default:
                std::cout << "Unknown pre-processing function" << std::endl;
                return;
        }

        // 调试输出：检查处理结果
        std::cout << "DEBUG: Processing result - empty: " << result.empty()
                  << ", size: " << result.cols << "x" << result.rows
                  << ", channels: " << result.channels() << std::endl;

        // 更新处理器中的图像 - 使用新的专用方法
        processor.applyPreProcessedImage(result);

        // 调试输出：确认图像已更新
        cv::Size newSize = processor.getImageSize();
        std::cout << "DEBUG: Image updated successfully. New size: " << newSize.width << "x" << newSize.height << std::endl;

        // 验证显示图像是否也更新了
        cv::Mat displayImg = processor.getDisplayImage();
        std::cout << "DEBUG: Display image - empty: " << displayImg.empty()
                  << ", size: " << displayImg.cols << "x" << displayImg.rows << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Error applying pre-processing function: " << e.what() << std::endl;
    }
}

// 具体的预处理算法实现
cv::Mat ImageProcessingApp::applyAdjustContrast(const cv::Mat& image, double brightness, double contrast) {
    cv::Mat result;
    image.convertTo(result, image.type(), contrast, brightness);
    std::cout << "DEBUG: applyAdjustContrast - input: " << image.cols << "x" << image.rows
              << ", output: " << result.cols << "x" << result.rows << std::endl;
    return result;
}

cv::Mat ImageProcessingApp::applyHistogramEqualization(const cv::Mat& image) {
    cv::Mat result;

    if (histogramMethod == 0) {
        // Global histogram equalization
        if (image.channels() == 1) {
            cv::equalizeHist(image, result);
        } else {
            cv::Mat ycrcb;
            cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
            std::vector<cv::Mat> channels;
            cv::split(ycrcb, channels);
            cv::equalizeHist(channels[0], channels[0]);
            cv::merge(channels, ycrcb);
            cv::cvtColor(ycrcb, result, cv::COLOR_YCrCb2BGR);
        }
    } else {
        // Adaptive histogram equalization (CLAHE)
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clipLimit, cv::Size(8, 8));

        if (image.channels() == 1) {
            clahe->apply(image, result);
        } else {
            cv::Mat ycrcb;
            cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
            std::vector<cv::Mat> channels;
            cv::split(ycrcb, channels);
            clahe->apply(channels[0], channels[0]);
            cv::merge(channels, ycrcb);
            cv::cvtColor(ycrcb, result, cv::COLOR_YCrCb2BGR);
        }
    }
    return result;
}

cv::Mat ImageProcessingApp::applyFlattenBackground(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(flattenKernelSize, flattenKernelSize));
    cv::morphologyEx(image, result, cv::MORPH_TOPHAT, kernel);
    return result;
}

cv::Mat ImageProcessingApp::applyMedianFilter(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::medianBlur(image, result, kernelSize);
    return result;
}

cv::Mat ImageProcessingApp::applyWienerFilter(const cv::Mat& image, int kernelSize) {
    // 简化的Wiener滤波实现 - 使用双边滤波近似
    cv::Mat result;
    cv::bilateralFilter(image, result, kernelSize, kernelSize * 2, kernelSize / 2);
    return result;
}

cv::Mat ImageProcessingApp::applyNonLocalMeans(const cv::Mat& image, double h, int templateWindowSize, int searchWindowSize) {
    cv::Mat result;
    if (image.channels() == 1) {
        cv::fastNlMeansDenoising(image, result, h, templateWindowSize, searchWindowSize);
    } else {
        cv::fastNlMeansDenoisingColored(image, result, h, h, templateWindowSize, searchWindowSize);
    }
    return result;
}

cv::Mat ImageProcessingApp::applyGaussianBlur(const cv::Mat& image, int kernelSize, double sigmaX, double sigmaY) {
    cv::Mat result;
    cv::GaussianBlur(image, result, cv::Size(kernelSize, kernelSize), sigmaX, sigmaY);
    return result;
}

cv::Mat ImageProcessingApp::applyAverageBlur(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::blur(image, result, cv::Size(kernelSize, kernelSize));
    return result;
}

cv::Mat ImageProcessingApp::applySumFilter(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::Mat::ones(kernelSize, kernelSize, CV_32F);
    cv::filter2D(image, result, -1, kernel);
    return result;
}

cv::Mat ImageProcessingApp::applyGrayscaleDilate(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::dilate(image, result, kernel);
    return result;
}

cv::Mat ImageProcessingApp::applyGrayscaleErode(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::erode(image, result, kernel);
    return result;
}

// 其余预处理算法的简化实现
cv::Mat ImageProcessingApp::applyStdDevFilter(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat mean, sqrMean;
    cv::blur(image, mean, cv::Size(kernelSize, kernelSize));
    cv::Mat squared;
    cv::multiply(image, image, squared);
    cv::blur(squared, sqrMean, cv::Size(kernelSize, kernelSize));
    cv::Mat meanSquared;
    cv::multiply(mean, mean, meanSquared);
    cv::sqrt(sqrMean - meanSquared, result);
    return result;
}

cv::Mat ImageProcessingApp::applyEntropyFilter(const cv::Mat& image, int kernelSize) {
    // 简化的熵滤波实现 - 使用局部标准差近似
    return applyStdDevFilter(image, kernelSize);
}

cv::Mat ImageProcessingApp::applyGradientFilter(const cv::Mat& image) {
    cv::Mat result;
    cv::Mat grad_x, grad_y;
    cv::Sobel(image, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(image, grad_y, CV_16S, 0, 1, 3);
    cv::convertScaleAbs(grad_x, grad_x);
    cv::convertScaleAbs(grad_y, grad_y);
    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, result);
    return result;
}

cv::Mat ImageProcessingApp::applyHighlightLines(const cv::Mat& image) {
    cv::Mat result;
    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150);

    if (image.channels() == 3) {
        cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
    } else {
        result = edges;
    }
    return result;
}

cv::Mat ImageProcessingApp::applyBrightTexture(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::Mat::ones(kernelSize, kernelSize, CV_32F) / (float)(kernelSize * kernelSize);
    cv::filter2D(image, result, -1, kernel);
    cv::subtract(image, result, result);
    cv::add(result, cv::Scalar::all(128), result);
    return result;
}

cv::Mat ImageProcessingApp::applyDarkTexture(const cv::Mat& image, int kernelSize) {
    cv::Mat result;
    cv::Mat kernel = cv::Mat::ones(kernelSize, kernelSize, CV_32F) / (float)(kernelSize * kernelSize);
    cv::filter2D(image, result, -1, kernel);
    cv::subtract(result, image, result);
    cv::add(result, cv::Scalar::all(128), result);
    return result;
}

cv::Mat ImageProcessingApp::applyAdvancedTexture(const cv::Mat& image, int kernelSize) {
    // 高级纹理增强 - 使用拉普拉斯算子
    cv::Mat result;
    cv::Mat laplacian;
    cv::Laplacian(image, laplacian, CV_16S, kernelSize);
    cv::convertScaleAbs(laplacian, result);
    return result;
}

cv::Mat ImageProcessingApp::applySimilarity(const cv::Mat& image, int kernelSize) {
    // 相似性滤波 - 使用双边滤波
    cv::Mat result;
    cv::bilateralFilter(image, result, kernelSize, kernelSize * 2, kernelSize / 2);
    return result;
}

cv::Mat ImageProcessingApp::applySharpen(const cv::Mat& image, double strength) {
    cv::Mat result;
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
                      0, -1 * strength, 0,
                      -1 * strength, 1 + 4 * strength, -1 * strength,
                      0, -1 * strength, 0);
    cv::filter2D(image, result, -1, kernel);
    return result;
}

cv::Mat ImageProcessingApp::applyFFTFilter(const cv::Mat& image) {
    // 简化的FFT滤波 - 使用高斯低通滤波近似
    cv::Mat result;
    cv::GaussianBlur(image, result, cv::Size(15, 15), 2.0, 2.0);
    return result;
}

cv::Mat ImageProcessingApp::applyGrayscaleInterpolation(const cv::Mat& image) {
    // 灰度插值 - 使用双线性插值放大后缩小
    cv::Mat result, temp;
    cv::resize(image, temp, cv::Size(image.cols * 2, image.rows * 2), 0, 0, cv::INTER_LINEAR);
    cv::resize(temp, result, image.size(), 0, 0, cv::INTER_LINEAR);
    return result;
}

cv::Mat ImageProcessingApp::applyGrayscaleReconstruction(const cv::Mat& image) {
    // 灰度重建 - 使用形态学重建的简化版本
    cv::Mat result;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(image, result, cv::MORPH_CLOSE, kernel);
    return result;
}