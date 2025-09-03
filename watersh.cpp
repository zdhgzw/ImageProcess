#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>

#include <sys/stat.h>
#include <sys/types.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// 辅助函数：创建目录
void createDirectoryIfNotExists(const std::string& path) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    if (stat(path.c_str(), &st) == -1) {
        mkdir(path.c_str(), 0777);
    }
}

// 辅助函数：为调试目的保存图像
void save_image(const cv::Mat& image, const std::string& name, const std::string& path) {
    std::string full_path = path + "/" + name;
    
    if (image.type() == CV_32S) {
        double min, max;
        cv::minMaxLoc(image, &min, &max);
        cv::Mat display_img;
        if (max > min) {
            image.convertTo(display_img, CV_8U, 255.0 / (max - min), -min * 255.0 / (max - min));
        } else {
            image.convertTo(display_img, CV_8U);
        }
        cv::applyColorMap(display_img, display_img, cv::COLORMAP_JET);
        display_img.setTo(cv::Scalar(0, 0, 0), image == 0);
        cv::imwrite(full_path, display_img);
    } else {
        cv::imwrite(full_path, image);
    }
    std::cout << "  -> 已保存调试图片: " << name << std::endl;
}

// 辅助函数：移除小面积对象
void removeSmallObjects(const cv::Mat& src, cv::Mat& dst, double min_area) {
    cv::Mat temp = src.clone();
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(temp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    dst = cv::Mat::zeros(src.size(), CV_8UC1);
    for (size_t i = 0; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) > min_area) {
            cv::drawContours(dst, contours, static_cast<int>(i), cv::Scalar(255), cv::FILLED);
        }
    }
}

// 辅助函数：从区域最大值生成种子点
cv::Mat generateSeedsFromRegionalMaxima(const cv::Mat& distTransform, double peakThreshold) {
    cv::Mat peaks_mask;
    cv::threshold(distTransform, peaks_mask, peakThreshold, 255, cv::THRESH_BINARY);
    peaks_mask.convertTo(peaks_mask, CV_8U);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(peaks_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat sure_fg = cv::Mat::zeros(peaks_mask.size(), CV_8U);
    for(const auto& contour : contours) {
        cv::Mat contour_mask = cv::Mat::zeros(peaks_mask.size(), CV_8U);
        cv::drawContours(contour_mask, std::vector<std::vector<cv::Point>>{contour}, -1, cv::Scalar(255), -1);
        
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(distTransform, &minVal, &maxVal, &minLoc, &maxLoc, contour_mask);
        
        sure_fg.at<unsigned char>(maxLoc) = 255;
    }

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(sure_fg, sure_fg, kernel);
    return sure_fg;
}


// ======================================================================
// 【最终修正版】辅助函数：填充孔洞
// ======================================================================
void fillHoles(const cv::Mat& src, cv::Mat& dst) {
    CV_Assert(src.type() == CV_8UC1);

    // findContours会修改输入，所以在一个副本上操作
    cv::Mat temp = src.clone();
    std::vector<std::vector<cv::Point>> contours;
    
    // 关键：只寻找最外层的轮廓
    cv::findContours(temp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // 在一个全新的黑色画布上，将所有找到的外轮廓完全填充
    dst = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::drawContours(dst, contours, -1, cv::Scalar(255), cv::FILLED);
}


// 流程一：主分割流程
std::pair<cv::Mat, int> runStage1_Watershed(const cv::Mat& imageRgb, const cv::Mat& imageGray, const std::string& outputDir) {
    std::cout << "\n---[ STAGE 1: 执行主分割流程 (分水岭) ]---\n";
    
    std::vector<int> kernel_sizes = {41, 91, 151}; 
    std::vector<cv::Mat> blackhat_results;
    for (size_t i = 0; i < kernel_sizes.size(); ++i) {
        int size = kernel_sizes[i];
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(size, size));
        cv::Mat blackhat_result;
        cv::morphologyEx(imageGray, blackhat_result, cv::MORPH_BLACKHAT, kernel);
        blackhat_results.push_back(blackhat_result);
    }
    cv::Mat fused_image = blackhat_results[0].clone();
    for (size_t i = 1; i < blackhat_results.size(); ++i) {
        cv::max(fused_image, blackhat_results[i], fused_image);
    }
    save_image(fused_image, "s1_01_fused_blackhat.png", outputDir);

    double strict_threshold_value = 40.0; 
    cv::Mat mask_precision;
    cv::threshold(fused_image, mask_precision, strict_threshold_value, 255, cv::THRESH_BINARY);
    cv::Mat cleaned_basin;
    removeSmallObjects(mask_precision, cleaned_basin, 250);
    save_image(cleaned_basin, "s1_02_precision_basin.png", outputDir);

    cv::Mat dist, dist_smoothed, sure_fg, markers;
    cv::distanceTransform(cleaned_basin, dist, cv::DIST_L2, 5);
    cv::normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);
    cv::GaussianBlur(dist, dist_smoothed, cv::Size(15, 15), 0);
    sure_fg = generateSeedsFromRegionalMaxima(dist_smoothed, 0.3);
    
    int nMarkers = cv::connectedComponents(sure_fg, markers);
    markers = markers + 1;
    
    cv::Mat sure_bg, unknown;
    cv::dilate(cleaned_basin, sure_bg, cv::Mat(), cv::Point(-1, -1), 3);
    cv::subtract(sure_bg, sure_fg, unknown);
    markers.setTo(0, unknown == 255);
    
    cv::Mat imageRgbForWatershed = imageRgb.clone();
    cv::watershed(imageRgbForWatershed, markers);
    markers.setTo(0, markers == -1);
    save_image(markers, "s1_03_watershed_result.png", outputDir);

    return {markers, nMarkers - 1};
}

// 流程二：“救援”流程
std::vector<std::vector<cv::Point>> runStage2_Rescue(const cv::Mat& imageGray, const cv::Mat& stage1_result_mask, const std::string& outputDir) {
    std::cout << "\n---[ STAGE 2: 执行救援任务 (查找遗漏细胞) ]---\n";
    
    cv::Mat imageGrayEnhanced;
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTileGridSize(cv::Size(8, 8));
    clahe->apply(imageGray, imageGrayEnhanced);

    cv::Mat mask_sensitive;
    cv::adaptiveThreshold(imageGrayEnhanced, mask_sensitive, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                          cv::THRESH_BINARY_INV, 85, 8);
    save_image(mask_sensitive, "s2_01_sensitive_mask_raw.png", outputDir);

    cv::Mat mask_sensitive_closed, mask_sensitive_filled, mask_sensitive_cleaned;
    cv::morphologyEx(mask_sensitive, mask_sensitive_closed, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7)));
    // 使用修正后的 fillHoles
    fillHoles(mask_sensitive_closed, mask_sensitive_filled);
    removeSmallObjects(mask_sensitive_filled, mask_sensitive_cleaned, 100);
    save_image(mask_sensitive_cleaned, "s2_02_sensitive_mask_cleaned.png", outputDir);

    cv::Mat mask_stage1_found;
    stage1_result_mask.convertTo(mask_stage1_found, CV_8U);
    cv::threshold(mask_stage1_found, mask_stage1_found, 0, 255, cv::THRESH_BINARY);

    cv::Mat missed_candidates;
    cv::subtract(mask_sensitive_cleaned, mask_stage1_found, missed_candidates);
    save_image(missed_candidates, "s2_03_missed_candidates.png", outputDir);

    std::vector<std::vector<cv::Point>> rescued_contours;
    std::vector<std::vector<cv::Point>> candidate_contours;
    cv::findContours(missed_candidates, candidate_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double minArea_rescue = 100;
    double maxArea_rescue = 8000;
    double minCircularity_rescue = 0.6; 

    for (const auto& contour : candidate_contours) {
        double area = cv::contourArea(contour);
        if (area < minArea_rescue || area > maxArea_rescue) continue;

        double perimeter = cv::arcLength(contour, true);
        if (perimeter == 0) continue;
        double circularity = (4 * CV_PI * area) / (perimeter * perimeter);
        if (circularity < minCircularity_rescue) continue;

        rescued_contours.push_back(contour);
    }
    std::cout << "  -> 救援任务找到 " << rescued_contours.size() << " 个被遗漏的细胞。\n";
    return rescued_contours;
}

int main() {
    std::string inputImagePath = "cell2.jpg";
    std::string outputDir = "cell_output_two_stage_final";
    createDirectoryIfNotExists(outputDir);

    cv::Mat imageRgb = cv::imread(inputImagePath, cv::IMREAD_COLOR);
    if (imageRgb.empty()) {
        std::cerr << "错误: 无法读取图片 '" << inputImagePath << "'.\n";
        return -1;
    }
    cv::Mat imageGray;
    cv::cvtColor(imageRgb, imageGray, cv::COLOR_BGR2GRAY);

    std::pair<cv::Mat, int> stage1_result = runStage1_Watershed(imageRgb, imageGray, outputDir);
    cv::Mat stage1_labels = stage1_result.first;
    int count_stage1 = stage1_result.second;

    std::vector<std::vector<cv::Point>> rescued_contours = runStage2_Rescue(imageGray, stage1_labels, outputDir);
    int count_stage2 = rescued_contours.size();

    std::cout << "\n---[ 3. 合并结果并可视化 ]---\n";
    int finalCellCount = count_stage1 + count_stage2;
    std::cout << "最终细胞计数结果: " << finalCellCount << " 个 (主流程: " << count_stage1 << ", 救援: " << count_stage2 << ")\n";

    cv::Mat finalResultImage = imageRgb.clone();
    
    cv::Scalar color_stage1 = cv::Scalar(0, 0, 255); // Red
    for (int label = 2; label <= count_stage1 + 1; ++label) {
        cv::Mat mask = stage1_labels == label;
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::drawContours(finalResultImage, contours, -1, color_stage1, 2);
    }

    cv::Scalar color_stage2 = cv::Scalar(0, 255, 0); // Green
    cv::drawContours(finalResultImage, rescued_contours, -1, color_stage2, 2);
    
    std::string stats_text = "Total Cells: " + std::to_string(finalCellCount);
    cv::putText(finalResultImage, stats_text, cv::Point(10, 40), 
               cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(255, 0, 128), 2);
    
    std::string final_filename = outputDir + "/final_result_with_rescue.png";
    cv::imwrite(final_filename, finalResultImage);
    std::cout << "成功！最终结果图已保存为: " << final_filename << "\n";

    return 0;
}