#include "Morphology.h"
#include <iostream>

Morphology::Morphology() {
}

Morphology::~Morphology() {
}

cv::Mat Morphology::getKernel(int kernelType, int kernelSize) {
    int morphShape;
    switch (kernelType) {
        case 0: morphShape = cv::MORPH_RECT; break;
        case 1: morphShape = cv::MORPH_ELLIPSE; break;
        case 2: morphShape = cv::MORPH_CROSS; break;
        default: morphShape = cv::MORPH_ELLIPSE; break;
    }
    return cv::getStructuringElement(morphShape, cv::Size(kernelSize, kernelSize));
}

// 形态学操作算法实现
cv::Mat Morphology::dilate(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::dilate(image, result, kernel);
    
    std::cout << "DEBUG: dilate applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::erode(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::erode(image, result, kernel);
    
    std::cout << "DEBUG: erode applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::dilateUniform(const cv::Mat& image, int pixelValue) {
    cv::Mat result;
    
    std::cout << "DEBUG: dilateUniform applied with pixelValue=" << pixelValue << std::endl;
    
    // Uniform膨胀：以指定像素值作为膨胀半径进行精确控制
    if (pixelValue <= 0) {
        result = image.clone();
        return result;
    }
    
    // 创建圆形结构元素，半径为pixelValue
    int kernelSize = pixelValue * 2 + 1;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    
    // 执行膨胀操作
    cv::dilate(image, result, kernel);
    
    std::cout << "DEBUG: dilateUniform completed with equivalent kernelSize=" << kernelSize << std::endl;
    return result;
}

cv::Mat Morphology::erodeUniform(const cv::Mat& image, int pixelValue) {
    cv::Mat result;
    
    std::cout << "DEBUG: erodeUniform applied with pixelValue=" << pixelValue << std::endl;
    
    // Uniform腐蚀：以指定像素值作为腐蚀半径进行精确控制
    if (pixelValue <= 0) {
        result = image.clone();
        return result;
    }
    
    // 创建圆形结构元素，半径为pixelValue
    int kernelSize = pixelValue * 2 + 1;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    
    // 执行腐蚀操作
    cv::erode(image, result, kernel);
    
    std::cout << "DEBUG: erodeUniform completed with equivalent kernelSize=" << kernelSize << std::endl;
    return result;
}

cv::Mat Morphology::opening(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_OPEN, kernel);
    
    std::cout << "DEBUG: opening applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::closing(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_CLOSE, kernel);
    
    std::cout << "DEBUG: closing applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::gradient(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_GRADIENT, kernel);
    
    std::cout << "DEBUG: gradient applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::topHat(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_TOPHAT, kernel);
    
    std::cout << "DEBUG: topHat applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::blackHat(const cv::Mat& image, int kernelSize, int kernelType) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    cv::morphologyEx(image, result, cv::MORPH_BLACKHAT, kernel);
    
    std::cout << "DEBUG: blackHat applied with kernelSize=" << kernelSize << ", kernelType=" << kernelType << std::endl;
    return result;
}

cv::Mat Morphology::dilateRetain(const cv::Mat& image, int kernelSize, int kernelType, int retainMethod) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    
    std::cout << "DEBUG: dilateRetain applied with kernelSize=" << kernelSize 
              << ", kernelType=" << kernelType << ", retainMethod=" << retainMethod << std::endl;
    
    switch (retainMethod) {
        case 0: // 标记点保留方法
            {
                // 1. 先进行标准膨胀
                cv::Mat dilated;
                cv::dilate(image, dilated, kernel);
                
                // 2. 创建保留区域掩码
                cv::Mat retainMask = cv::Mat::zeros(image.size(), CV_8UC1);
                
                // 查找原始轮廓作为保留参考
                std::vector<std::vector<cv::Point>> contours;
                cv::Mat imageClone = image.clone(); // 避免修改原图
                cv::findContours(imageClone, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                
                // 在保留掩码上标记重要区域（对象中心点）
                for (const auto& contour : contours) {
                    cv::Moments moments = cv::moments(contour);
                    if (moments.m00 > 0) {
                        cv::Point2f center(moments.m10 / moments.m00, moments.m01 / moments.m00);
                        int radius = std::max(1, kernelSize / 3);
                        cv::circle(retainMask, center, radius, cv::Scalar(255), -1);
                    }
                }
                
                // 3. 合并结果：在保留区域使用原图，其他区域使用膨胀结果
                result = dilated.clone();
                image.copyTo(result, retainMask);
            }
            break;
            
        case 1: // 骨架保留方法
            {
                // 1. 简化的骨架提取（使用距离变换）
                cv::Mat distTransform;
                cv::distanceTransform(image, distTransform, cv::DIST_L2, 5);
                
                // 2. 提取局部最大值作为骨架
                cv::Mat localMaxima;
                cv::Mat kernel3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
                cv::dilate(distTransform, localMaxima, kernel3x3);
                cv::compare(distTransform, localMaxima, localMaxima, cv::CMP_EQ);
                
                // 3. 执行标准膨胀
                cv::Mat dilated;
                cv::dilate(image, dilated, kernel);
                
                // 4. 将骨架信息叠加到膨胀结果上
                cv::Mat skeleton;
                localMaxima.convertTo(skeleton, CV_8UC1);
                cv::bitwise_and(skeleton, image, skeleton); // 只保留原始对象内的骨架
                cv::bitwise_or(dilated, skeleton, result);
            }
            break;
            
        case 2: // 尺寸约束保留方法
            {
                // 1. 分析原始对象的尺寸分布
                std::vector<std::vector<cv::Point>> contours;
                cv::Mat imageClone = image.clone();
                cv::findContours(imageClone, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                
                if (contours.empty()) {
                    result = image.clone();
                    break;
                }
                
                // 2. 计算平均对象尺寸
                double avgArea = 0;
                for (const auto& contour : contours) {
                    avgArea += cv::contourArea(contour);
                }
                avgArea /= contours.size();
                
                // 3. 执行标准膨胀
                cv::Mat dilated;
                cv::dilate(image, dilated, kernel);
                
                // 4. 分析膨胀后的对象
                std::vector<std::vector<cv::Point>> dilatedContours;
                cv::Mat dilatedClone = dilated.clone();
                cv::findContours(dilatedClone, dilatedContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                
                result = cv::Mat::zeros(image.size(), CV_8UC1);
                for (const auto& contour : dilatedContours) {
                    double area = cv::contourArea(contour);
                    if (area <= avgArea * 2.5) { // 不超过原始平均尺寸的2.5倍
                        cv::drawContours(result, std::vector<std::vector<cv::Point>>{contour}, -1, cv::Scalar(255), cv::FILLED);
                    } else {
                        // 对过大的对象，使用更小的膨胀
                        cv::Rect boundingRect = cv::boundingRect(contour);
                        cv::Mat roi = image(boundingRect).clone();
                        if (!roi.empty()) {
                            cv::Mat roiDilated;
                            int smallSize = std::max(3, kernelSize / 2);
                            if (smallSize % 2 == 0) smallSize++;
                            cv::Mat smallKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(smallSize, smallSize));
                            cv::dilate(roi, roiDilated, smallKernel);
                            roiDilated.copyTo(result(boundingRect));
                        }
                    }
                }
            }
            break;
            
        default: // 默认使用标记点保留方法
            result = dilateRetain(image, kernelSize, kernelType, 0);
            break;
    }
    
    return result;
}

cv::Mat Morphology::erodeRetain(const cv::Mat& image, int kernelSize, int kernelType, int retainMethod) {
    cv::Mat result;
    cv::Mat kernel = getKernel(kernelType, kernelSize);
    
    std::cout << "DEBUG: erodeRetain applied with kernelSize=" << kernelSize 
              << ", kernelType=" << kernelType << ", retainMethod=" << retainMethod << std::endl;
    
    switch (retainMethod) {
        case 0: // 标记点保留方法
            {
                // 1. 执行标准腐蚀
                cv::Mat eroded;
                cv::erode(image, eroded, kernel);
                
                // 2. 找到原始图像的重要特征区域
                std::vector<std::vector<cv::Point>> contours;
                cv::Mat imageClone = image.clone();
                cv::findContours(imageClone, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                
                // 3. 创建保留掩码，保护重要的连接点
                cv::Mat retainMask = cv::Mat::zeros(image.size(), CV_8UC1);
                
                for (const auto& contour : contours) {
                    double area = cv::contourArea(contour);
                    if (area > kernelSize * kernelSize) { // 只保护足够大的对象
                        cv::Moments moments = cv::moments(contour);
                        if (moments.m00 > 0) {
                            cv::Point2f center(moments.m10 / moments.m00, moments.m01 / moments.m00);
                            int radius = std::max(2, kernelSize / 4);
                            cv::circle(retainMask, center, radius, cv::Scalar(255), -1);
                        }
                    }
                }
                
                // 4. 合并结果：主要使用腐蚀结果，但在关键点保留原始特征
                result = eroded.clone();
                image.copyTo(result, retainMask);
            }
            break;
            
        case 1: // 连通性保留方法
            {
                // 1. 执行标准腐蚀
                cv::Mat eroded;
                cv::erode(image, eroded, kernel);
                
                // 2. 分析连通性损失
                int originalComponents = 0, erodedComponents = 0;
                cv::Mat labels1, labels2;
                originalComponents = cv::connectedComponents(image, labels1);
                erodedComponents = cv::connectedComponents(eroded, labels2);
                
                // 3. 如果连通性损失严重，进行补偿
                if (erodedComponents < originalComponents * 0.7) {
                    // 使用更小的核进行腐蚀
                    int smallSize = std::max(3, kernelSize / 2);
                    if (smallSize % 2 == 0) smallSize++;
                    cv::Mat smallerKernel = cv::getStructuringElement(kernelType, cv::Size(smallSize, smallSize));
                    cv::erode(image, result, smallerKernel);
                } else {
                    result = eroded.clone();
                }
            }
            break;
            
        case 2: // 渐进式腐蚀保留方法
            {
                // 1. 分步骤进行腐蚀，每步检查特征保留情况
                result = image.clone();
                cv::Mat smallKernel = cv::getStructuringElement(kernelType, cv::Size(3, 3));
                
                int steps = kernelSize / 2;
                for (int i = 0; i < steps; i++) {
                    cv::Mat tempEroded;
                    cv::erode(result, tempEroded, smallKernel);
                    
                    // 检查是否还有足够的特征
                    std::vector<std::vector<cv::Point>> contours;
                    cv::findContours(tempEroded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                    
                    if (contours.size() > 0) {
                        result = tempEroded.clone();
                    } else {
                        break; // 停止腐蚀以保留最后的特征
                    }
                }
            }
            break;
            
        default: // 默认使用标记点保留方法
            result = erodeRetain(image, kernelSize, kernelType, 0);
            break;
    }
    
    return result;
}

// cv::Mat Morphology::separateFeatures(const cv::Mat& image, double edgeThreshold, int separationMethod, int kernelSize) {
//     cv::Mat result;
//     cv::Mat grayImage;

//     std::cout << "DEBUG: separateFeatures input - size=" << image.size() << ", channels=" << image.channels() << std::endl;

//     // Use input image directly (assuming it's already in the correct format)
//     if (image.channels() == 1) {
//         grayImage = image.clone();
//     } else if (image.channels() == 3) {
//         cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
//     } else {
//         grayImage = image.clone();
//     }

//     cv::Mat edges;

//     // Apply edge detection based on separation method
//     switch (separationMethod) {
//         case 0: // Canny edge detection
//             cv::Canny(grayImage, edges, edgeThreshold * 0.5, edgeThreshold, 3);
//             break;
//         case 1: // Sobel edge detection
//             {
//                 cv::Mat grad_x, grad_y;
//                 cv::Sobel(grayImage, grad_x, CV_16S, 1, 0, 3);
//                 cv::Sobel(grayImage, grad_y, CV_16S, 0, 1, 3);
//                 cv::convertScaleAbs(grad_x, grad_x);
//                 cv::convertScaleAbs(grad_y, grad_y);
//                 cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edges);
//                 cv::threshold(edges, edges, edgeThreshold, 255, cv::THRESH_BINARY);
//             }
//             break;
//         case 2: // Laplacian edge detection
//             cv::Laplacian(grayImage, edges, CV_8U, 3);
//             cv::threshold(edges, edges, edgeThreshold, 255, cv::THRESH_BINARY);
//             break;
//         default:
//             cv::Canny(grayImage, edges, edgeThreshold * 0.5, edgeThreshold, 3);
//             break;
//     }

//     // Apply morphological operations to separate features
//     cv::Mat kernel = getKernel(1, kernelSize); // Use ellipse kernel
//     cv::morphologyEx(edges, result, cv::MORPH_CLOSE, kernel);

//     // Find contours and draw separated features
//     std::vector<std::vector<cv::Point>> contours;
//     cv::findContours(result, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

//     // Create output image
//     result = cv::Mat::zeros(image.size(), CV_8UC3);

//     // Draw each contour with different colors for separation
//     for (size_t i = 0; i < contours.size(); i++) {
//         cv::Scalar color = cv::Scalar(
//             (i * 50) % 255,
//             (i * 100) % 255,
//             (i * 150) % 255
//         );
//         cv::drawContours(result, contours, (int)i, color, 2);
//         cv::fillPoly(result, contours, color);
//     }

//     std::cout << "DEBUG: separateFeatures applied with edgeThreshold=" << edgeThreshold
//               << ", separationMethod=" << separationMethod << ", kernelSize=" << kernelSize
//               << ", found " << contours.size() << " features" << std::endl;
//     return result;
// }

// 统一的应用函数
cv::Mat Morphology::applyFunction(const cv::Mat& image, MorphologyFunction function, const std::vector<double>& params) {
    if (image.channels() != 1) {
        std::cout << "DEBUG: Morphology applyFunction must be single-channel" << std::endl;
        return image.clone();
    }
    int kernelSize = params.size() > 0 ? (int)params[0] : 5;
    int kernelType = params.size() > 1 ? (int)params[1] : 1; // Default to ELLIPSE
    
    switch (function) {
        case MorphologyFunction::DILATE:
            return dilate(image, kernelSize, kernelType);
        case MorphologyFunction::ERODE:
            return erode(image, kernelSize, kernelType);
        case MorphologyFunction::OPENING:
            return opening(image, kernelSize, kernelType);
        case MorphologyFunction::CLOSING:
            return closing(image, kernelSize, kernelType);
        case MorphologyFunction::GRADIENT:
            return gradient(image, kernelSize, kernelType);
        case MorphologyFunction::TOP_HAT:
            return topHat(image, kernelSize, kernelType);
        case MorphologyFunction::BLACK_HAT:
            return blackHat(image, kernelSize, kernelType);
        case MorphologyFunction::DILATE_UNIFORM:
            {
                int pixelValue = params.size() > 5 ? (int)params[5] : kernelSize;
                return dilateUniform(image, pixelValue);
            }
        case MorphologyFunction::ERODE_UNIFORM:
            {
                int pixelValue = params.size() > 5 ? (int)params[5] : kernelSize;
                return erodeUniform(image, pixelValue);
            }
        case MorphologyFunction::DILATE_RETAIN:
            {
                int retainMethod = params.size() > 4 ? (int)params[4] : 0;
                return dilateRetain(image, kernelSize, kernelType, retainMethod);
            }
        case MorphologyFunction::ERODE_RETAIN:
            {
                int retainMethod = params.size() > 4 ? (int)params[4] : 0;
                return erodeRetain(image, kernelSize, kernelType, retainMethod);
            }
        default:
            return image.clone();
    }
}
