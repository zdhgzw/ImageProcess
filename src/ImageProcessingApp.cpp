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
    // 创建主窗口
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    
    std::cout << "图像处理应用程序已启动" << std::endl;
    std::cout << "支持的功能:" << std::endl;
    std::cout << "1. 图像加载与显示" << std::endl;
    std::cout << "2. 彩色图像处理" << std::endl;
    std::cout << "按键说明:" << std::endl;
    std::cout << "  L - 加载图像" << std::endl;
    std::cout << "  R - 重置到原始图像" << std::endl;
    std::cout << "  G - 转换为灰度" << std::endl;
    std::cout << "  C - 颜色聚类 (K=3)" << std::endl;
    std::cout << "  1,2,3 - 颜色反褶积 (蓝、绿、红通道)" << std::endl;
    std::cout << "  ESC - 退出" << std::endl;
}

void ImageProcessingApp::initializeUI() {
    windowWidth = 800;
    windowHeight = 600;
    
    // 初始化K-means参数
    k_clusters = 3;
    
    // 初始化通道选择
    selected_channel = 0;
    
    // 创建主界面Mat
    frame = cv::Mat::zeros(windowHeight, windowWidth, CV_8UC3);
}

void ImageProcessingApp::run() {
    while (true) {
        // 显示当前图像或空白画布
        if (processor.hasImage()) {
            cv::Mat displayImg = scaleImageToFit(processor.getDisplayImage());
            if (!displayImg.empty()) {
                cv::imshow(windowName, displayImg);
            }
        } else {
            // 显示提示信息
            cv::Mat infoFrame = cv::Mat::zeros(300, 600, CV_8UC3);
            cv::putText(infoFrame, "No image loaded", 
                       cv::Point(200, 150), 
                       cv::FONT_HERSHEY_SIMPLEX, 1, 
                       cv::Scalar(255, 255, 255), 2);
            cv::putText(infoFrame, "Press 'L' to load image", 
                       cv::Point(150, 200), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, 
                       cv::Scalar(200, 200, 200), 2);
            cv::imshow(windowName, infoFrame);
        }
        
        // 检查按键
        int key = cv::waitKey(30) & 0xFF;
        
        if (key == 27) { // ESC键退出
            break;
        } else if (key == 'l' || key == 'L') {
            // 加载图像
            std::string filepath = openFileDialog();
            if (!filepath.empty()) {
                if (processor.loadImage(filepath)) {
                    std::cout << "成功加载图像: " << filepath << std::endl;
                } else {
                    std::cout << "加载图像失败: " << filepath << std::endl;
                }
            }
        } else if (key == 'r' || key == 'R') {
            // 重置图像
            processor.resetToOriginal();
        } else if (key == 'g' || key == 'G') {
            // 转换为灰度
            try {
                processor.convertToGrayscale();
            } catch (const std::exception& e) {
                std::cout << "错误: " << e.what() << std::endl;
            }
        } else if (key == 'c' || key == 'C') {
            // 颜色聚类
            try {
                processor.colorCluster(k_clusters);
            } catch (const std::exception& e) {
                std::cout << "错误: " << e.what() << std::endl;
            }
        } else if (key == '1') {
            // 蓝色通道
            try {
                processor.colorDeconvolution(0);
            } catch (const std::exception& e) {
                std::cout << "错误: " << e.what() << std::endl;
            }
        } else if (key == '2') {
            // 绿色通道
            try {
                processor.colorDeconvolution(1);
            } catch (const std::exception& e) {
                std::cout << "错误: " << e.what() << std::endl;
            }
        } else if (key == '3') {
            // 红色通道
            try {
                processor.colorDeconvolution(2);
            } catch (const std::exception& e) {
                std::cout << "错误: " << e.what() << std::endl;
            }
        }
    }
    
    cv::destroyAllWindows();
}

void ImageProcessingApp::renderUI() {
    // 简化版本不需要复杂UI渲染
}

void ImageProcessingApp::renderImageLoadPanel() {
    // 简化版本不需要复杂UI渲染
}

void ImageProcessingApp::renderColorProcessingPanel() {
    // 简化版本不需要复杂UI渲染
}

void ImageProcessingApp::renderImageDisplay() {
    // 简化版本不需要复杂UI渲染
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
    }
#endif
    
    // 如果不是Windows或者对话框取消，返回空字符串
    std::cout << "请在控制台输入图像文件路径: ";
    std::string path;
    std::getline(std::cin, path);
    return path;
}

cv::Mat ImageProcessingApp::scaleImageToFit(const cv::Mat& image) {
    if (image.empty()) {
        return cv::Mat();
    }
    
    int maxWidth = 800;
    int maxHeight = 600;
    
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