#include "Segmentation.h"
#include <iostream>
#include <vector>
#include <set>

Segmentation::Segmentation() {
}

Segmentation::~Segmentation() {
}

// THRESHOLD类别算法实现
cv::Mat Segmentation::basicThreshold(const cv::Mat& image, double threshold, int type) {
    cv::Mat result;

    std::cout << "DEBUG: basicThreshold input - size=" << image.size() << ", channels=" << image.channels() << std::endl;

    // Apply threshold directly based on input image type
    int thresholdType = (type == 0) ? cv::THRESH_BINARY : cv::THRESH_BINARY_INV;
    cv::threshold(image, result, threshold, 255, thresholdType);

    std::cout << "DEBUG: basicThreshold applied with threshold=" << threshold << ", type=" << type
              << ", result channels=" << result.channels() << std::endl;
    return result;
}

cv::Mat Segmentation::rangeThreshold(const cv::Mat& image, double minVal, double maxVal) {
    cv::Mat result;
    // Apply range threshold using inRange
    cv::inRange(image, cv::Scalar(minVal), cv::Scalar(maxVal), result);

    std::cout << "DEBUG: rangeThreshold applied with minVal=" << minVal << ", maxVal=" << maxVal
              << ", result channels=" << result.channels() << std::endl;
    return result;
}

cv::Mat Segmentation::adaptiveThreshold(const cv::Mat& image, int method, int type, int blockSize, double C) {
    cv::Mat result;
    // Ensure block size is odd and >= 3
    if (blockSize % 2 == 0) blockSize++;
    if (blockSize < 3) blockSize = 3;
    
    // Apply adaptive threshold
    int adaptiveMethod = (method == 0) ? cv::ADAPTIVE_THRESH_MEAN_C : cv::ADAPTIVE_THRESH_GAUSSIAN_C;
    int thresholdType = (type == 0) ? cv::THRESH_BINARY : cv::THRESH_BINARY_INV;
    
    cv::adaptiveThreshold(image, result, 255, adaptiveMethod, thresholdType, blockSize, C);

    std::cout << "DEBUG: adaptiveThreshold applied with method=" << method << ", type=" << type
              << ", blockSize=" << blockSize << ", C=" << C << ", result channels=" << result.channels() << std::endl;
    return result;
}

cv::Mat Segmentation::otsuThreshold(const cv::Mat& image) {
    cv::Mat result;
    cv::Mat grayImage;
    //otsu
    cv::threshold(image, result, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    
    std::cout << "DEBUG: otsuThreshold applied, result channels=" << result.channels() << std::endl;
    return result;
}

cv::Mat Segmentation::localThreshold(const cv::Mat& image, int blockSize, double C) {
    // Use adaptive threshold as a simplified local threshold implementation
    return adaptiveThreshold(image, 1, 0, blockSize, C); // Gaussian method, binary
}

/**
 * @brief 分水岭分割
 * @param image 输入图像（预处理结果，灰度图）
 * @param distanceThreshold 距离变换阈值（0.1-0.9，控制前景区域大小）
 * @param minArea 最小区域面积（过滤小区域）
 * @return 在输入图像基础上标记分割结果的彩色图像
 * 这些操作都必须在干净的二值图像上进行。 没有好的二值图像，
 * 就无法计算出前景像素到背景的距离，也就无法找到每个粘连颗粒的中心，分水岭算法就无从谈起
 */
cv::Mat Segmentation::watershed(const cv::Mat& image, const cv::Mat& originalImage, double distanceThreshold, bool showVisualization) {
    if (image.empty() || originalImage.empty()) {
        std::cout << "ERROR: watershed received empty image" << std::endl;
        return originalImage.empty() ? image.clone() : originalImage.clone();
    }

    std::cout << "DEBUG: Watershed input - processed image size=" << image.size()
              << ", channels=" << image.channels()
              << ", original image size=" << originalImage.size()
              << ", channels=" << originalImage.channels()
              << ", distanceThreshold=" << distanceThreshold
              << ", showVisualization=" << (showVisualization ? "true" : "false") << std::endl;

    // 输入应该是预处理后的二值图像（串行流程）
    if (image.channels() != 1) {
        std::cout << "ERROR: Watershed expects single-channel binary image from previous processing steps" << std::endl;
        return originalImage.clone();
    }

    // 直接使用输入的二值图像（已经过预处理）
    cv::Mat binary = image.clone();
    std::cout << "DEBUG: Using preprocessed binary image directly (serial workflow)" << std::endl;

    // 创建可视化画布 (2x4网格显示各个步骤，包含最终结果)
    cv::Mat canvas;
    int canvasWidth = 1600;
    int canvasHeight = 800;
    int stepWidth = canvasWidth / 4;
    int stepHeight = canvasHeight / 2;

    if (showVisualization) {
        canvas = cv::Mat::zeros(canvasHeight, canvasWidth, CV_8UC3);
        std::cout << "DEBUG: Creating visualization canvas: " << canvasWidth << "x" << canvasHeight << std::endl;
    }

    // 跳过显示输入二值图像，直接从步骤2开始

    std::cout << "DEBUG: Step 1 - Using preprocessed binary image (serial workflow)" << std::endl;

    // 2. 确定背景区域（膨胀操作）
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat sure_bg;
    cv::dilate(binary, sure_bg, kernel, cv::Point(-1, -1), 3);
    std::cout << "DEBUG: Step 2 - Background dilation completed" << std::endl;

    // 添加步骤2到画布: 确定背景
    if (showVisualization) {
        cv::Mat step2Display;
        cv::cvtColor(sure_bg, step2Display, cv::COLOR_GRAY2BGR);
        cv::resize(step2Display, step2Display, cv::Size(stepWidth-10, stepHeight-40));
        step2Display.copyTo(canvas(cv::Rect(5, 25, stepWidth-10, stepHeight-40)));
        cv::putText(canvas, "1. Sure Background", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    // 3. 距离变换确定前景区域
    cv::Mat dist_transform;
    cv::distanceTransform(binary, dist_transform, cv::DIST_L2, 5);
    cv::normalize(dist_transform, dist_transform, 0, 1.0, cv::NORM_MINMAX);
    std::cout << "DEBUG: Step 3 - Distance transform completed" << std::endl;

    // 添加步骤3到画布: 距离变换
    if (showVisualization) {
        cv::Mat step3Display;
        cv::Mat distDisplay;
        dist_transform.convertTo(distDisplay, CV_8U, 255.0);
        cv::cvtColor(distDisplay, step3Display, cv::COLOR_GRAY2BGR);
        cv::resize(step3Display, step3Display, cv::Size(stepWidth-10, stepHeight-40));
        step3Display.copyTo(canvas(cv::Rect(stepWidth+5, 25, stepWidth-10, stepHeight-40)));
        cv::putText(canvas, "2. Distance Transform", cv::Point(stepWidth+10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    // 5. 基于相对阈值获取确定的前景区域
    cv::Mat sure_fg;
    cv::threshold(dist_transform, sure_fg, distanceThreshold, 1.0, cv::THRESH_BINARY);
    sure_fg.convertTo(sure_fg, CV_8U);
    sure_fg = sure_fg * 255; // 转换为0-255范围
    std::cout << "DEBUG: Step 5 - Foreground threshold completed with threshold=" << distanceThreshold << std::endl;

    // 检查前景区域是否为空
    int foregroundPixels = cv::countNonZero(sure_fg);
    std::cout << "DEBUG: Foreground pixels found: " << foregroundPixels << std::endl;

    // 添加步骤4到画布: 确定前景
    if (showVisualization) {
        cv::Mat step4Display;
        cv::cvtColor(sure_fg, step4Display, cv::COLOR_GRAY2BGR);
        cv::resize(step4Display, step4Display, cv::Size(stepWidth-10, stepHeight-40));
        step4Display.copyTo(canvas(cv::Rect(stepWidth*2+5, 25, stepWidth-10, stepHeight-40)));
        cv::putText(canvas, "3. Sure Foreground", cv::Point(stepWidth*2+10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    // 6. 计算未知区域 = 背景 - 前景
    cv::Mat unknown;
    cv::subtract(sure_bg, sure_fg, unknown);
    std::cout << "DEBUG: Step 6 - Unknown region calculated" << std::endl;

    // 添加步骤5到画布: 未知区域
    if (showVisualization) {
        cv::Mat step5Display;
        cv::cvtColor(unknown, step5Display, cv::COLOR_GRAY2BGR);
        cv::resize(step5Display, step5Display, cv::Size(stepWidth-10, stepHeight-40));
        step5Display.copyTo(canvas(cv::Rect(5, stepHeight+25, stepWidth-10, stepHeight-40)));
        cv::putText(canvas, "4. Unknown Region", cv::Point(10, stepHeight+20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }
    
    // 7. 使用connectedComponents进行连通区域标记（标准方法）
    cv::Mat markers;
    int numLabels = cv::connectedComponents(sure_fg, markers);
    std::cout << "DEBUG: Step 7 - Connected components found: " << numLabels << " labels" << std::endl;
    
    // 8. 标准分水岭标记处理
    // 所有标记加1，确保背景从1开始（标准做法）
    markers = markers + 1;

    // 将未知区域标记为0（分水岭算法要求）
    markers.setTo(0, unknown);

    // 检查标记图像的统计信息
    double minMarkerVal, maxMarkerVal;
    cv::minMaxLoc(markers, &minMarkerVal, &maxMarkerVal);
    std::cout << "DEBUG: Final markers range: " << minMarkerVal << " to " << maxMarkerVal << std::endl;
    std::cout << "DEBUG: Unknown pixels: " << cv::countNonZero(unknown) << std::endl;
    
    // 9. 使用原始彩色图像执行分水岭算法
    cv::Mat colorImage;
    if (originalImage.channels() != 3) {
        std::cout << "ERROR: Watershed requires 3-channel color image" << std::endl;
        return originalImage.clone();
    } 
    colorImage = originalImage.clone();
    std::cout << "DEBUG: Using original color image for watershed algorithm" << std::endl;

    // 10. 执行分水岭算法
    cv::watershed(colorImage, markers);
    std::cout << "DEBUG: Step 10 - Watershed algorithm completed" << std::endl;

    // 11. 生成结果图像，使用颜色填充分割区域
    cv::Mat result = cv::Mat::zeros(colorImage.size(), CV_8UC3);

    // 统计边界像素数量和对象数量
    int boundaryCount = 0;
    std::set<int> uniqueLabels;

    // 生成随机颜色表，为每个分割区域分配不同颜色
    std::vector<cv::Vec3b> colors;
    colors.push_back(cv::Vec3b(0, 0, 0)); // 背景色（黑色）
    colors.push_back(cv::Vec3b(64, 64, 64)); // 背景色（深灰）
    
    // 为每个对象生成明亮的随机颜色
    cv::RNG rng(12345);
    for (int i = 2; i < 256; i++) {
        // 生成明亮、饱和度高的颜色，便于区分
        int b = rng.uniform(100, 255);
        int g = rng.uniform(100, 255);  
        int r = rng.uniform(100, 255);
        colors.push_back(cv::Vec3b(b, g, r));
    }

    // 使用颜色填充每个分割区域
    for (int i = 0; i < markers.rows; i++) {
        for (int j = 0; j < markers.cols; j++) {
            int label = markers.at<int>(i, j);
            if (label == -1) {
                // 分水岭边界用白色表示，清晰分隔各区域
                boundaryCount++;
                result.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
            } else if (label > 0 && label < colors.size()) {
                // 用预定义颜色填充分割区域
                result.at<cv::Vec3b>(i, j) = colors[label];
                if (label > 1) {
                    // 统计对象标签（排除背景标签1）
                    uniqueLabels.insert(label);
                }
            }
        }
    }
    int objectCount = uniqueLabels.size();
    
    // 13. 添加最终结果到画布并显示
    if (showVisualization) {
        // 添加步骤5: 最终分割结果
        cv::Mat step5Display = result.clone();
        cv::resize(step5Display, step5Display, cv::Size(stepWidth-10, stepHeight-40));
        step5Display.copyTo(canvas(cv::Rect(stepWidth+5, stepHeight+25, stepWidth-10, stepHeight-40)));
        cv::putText(canvas, "5. Watershed Result", cv::Point(stepWidth+10, stepHeight+20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        
        // 添加步骤6: 原始彩色图像
        cv::Mat step6Display = originalImage.clone();
        cv::resize(step6Display, step6Display, cv::Size(stepWidth-10, stepHeight-40));
        step6Display.copyTo(canvas(cv::Rect(stepWidth*2+5, stepHeight+25, stepWidth-10, stepHeight-40)));
        cv::putText(canvas, "6. Original Image", cv::Point(stepWidth*2+10, stepHeight+20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

        // 添加统计信息到画布（移动到第7位置）
        cv::Mat infoArea = canvas(cv::Rect(stepWidth*3+5, stepHeight+25, stepWidth-10, stepHeight-40));
        infoArea.setTo(cv::Scalar(50, 50, 50)); // 深灰色背景

        cv::putText(canvas, "7. Statistics", cv::Point(stepWidth*3+10, stepHeight+20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

        int textY = stepHeight + 50;
        cv::putText(canvas, "Statistics:", cv::Point(stepWidth*3+15, textY), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);
        textY += 30;
        cv::putText(canvas, ("Components: " + std::to_string(numLabels)).c_str(), cv::Point(stepWidth*3+15, textY), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        textY += 25;
        cv::putText(canvas, ("Objects: " + std::to_string(objectCount)).c_str(), cv::Point(stepWidth*3+15, textY), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        textY += 25;
        cv::putText(canvas, ("Boundaries: " + std::to_string(boundaryCount)).c_str(), cv::Point(stepWidth*3+15, textY), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        textY += 25;
        cv::putText(canvas, ("Threshold: " + std::to_string(distanceThreshold).substr(0, 4)).c_str(), cv::Point(stepWidth*3+15, textY), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        
        cv::imshow("Watershed Process Visualization", canvas);
        std::cout << "DEBUG: Visualization canvas displayed with final result" << std::endl;
    }
    
    // 14. 输出最终统计信息
    std::cout << "INFO: Watershed segmentation completed:" << std::endl;
    std::cout << "  - Input processed: " << (image.channels() == 1 ? "grayscale" : "color") << std::endl;
    std::cout << "  - Distance threshold: " << distanceThreshold << std::endl;
    std::cout << "  - Connected components: " << numLabels << std::endl;
    std::cout << "  - Boundary pixels found: " << boundaryCount << std::endl;
    std::cout << "  - Objects segmented: " << objectCount << std::endl;
    std::cout << "  - Visualization canvas displayed in separate window" << std::endl;

    return result;
}

// 统一的应用函数
cv::Mat Segmentation::applyFunction(const cv::Mat& image, SegmentationFunction function, const std::vector<double>& params) {
    // 所有分割函数都要求单通道输入
    if (image.channels() != 1) {
        std::cout << "DEBUG: Segmentation applyFunction must be single-channel" << std::endl;
        return image.clone();
    }

    switch (function) {
        case SegmentationFunction::BASIC_THRESHOLD:
            return basicThreshold(image, params.size() > 0 ? params[0] : 127.0, params.size() > 1 ? (int)params[1] : 0);
        case SegmentationFunction::RANGE_THRESHOLD:
            return rangeThreshold(image, params.size() > 0 ? params[0] : 50.0, params.size() > 1 ? params[1] : 200.0);
        case SegmentationFunction::ADAPTIVE_THRESHOLD:
            return adaptiveThreshold(image, params.size() > 0 ? (int)params[0] : 0,
                                   params.size() > 1 ? (int)params[1] : 0,
                                   params.size() > 2 ? (int)params[2] : 11,
                                   params.size() > 3 ? params[3] : 2.0);
        case SegmentationFunction::OTSU_THRESHOLD:
            return otsuThreshold(image);
        case SegmentationFunction::LOCAL_THRESHOLD:
            return localThreshold(image, params.size() > 0 ? (int)params[0] : 11, params.size() > 1 ? params[1] : 2.0);
        case SegmentationFunction::WATERSHED:
            // 分水岭算法需要原始图像参数，applyFunction无法提供
            // 应该直接调用Segmentation::watershed函数，而不是通过applyFunction
            std::cout << "ERROR: Watershed algorithm should not be called through applyFunction" << std::endl;
            std::cout << "       Use Segmentation::watershed(processedImage, originalImage, threshold, showViz) directly" << std::endl;
            return image.clone();
        default:
            return image.clone();
    }
}
