#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

// Structure to store scale bar candidate information
struct ScaleBarCandidate {
    cv::Rect rect;
    double aspect_ratio;
    double score = 0.0;
};

/**
 * @brief Automatically detect scale bar and calculate microns per pixel (Version v9: Find longest valid line segment)
 * @details V9 version uses "longest valid line" strategy:
 * 1. Find all contours that match "elongated horizontal line" characteristics as candidates.
 * 2. Score each candidate based on: shape, position, and background brightness in original image.
 * 3. Select the candidate with highest score as the final scale bar.
 */
double getMicronPerPixel(const cv::Mat &image, double scale_bar_real_length_um, cv::Mat &debug_image) {
    std::cout << "--- Starting Scale Calibration (v9: Finding Longest Valid Line Segment) ---\n";

    // STAGE 1 & 2: Image preprocessing and directional opening operation
    cv::Mat gray, inverted_gray, binary_inverted;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::bitwise_not(gray, inverted_gray);
    cv::threshold(inverted_gray, binary_inverted, 50, 255, cv::THRESH_BINARY);

    cv::Mat horizontal_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(30, 1));
    cv::Mat line_only_image;
    cv::morphologyEx(binary_inverted, line_only_image, cv::MORPH_OPEN, horizontal_kernel, cv::Point(-1, -1), 2);
    cv::imshow("[Debug] Horizontal Opening Result", line_only_image);

    // STAGE 3: Find the widest contour among those that passed the line filter
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(line_only_image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        std::cout << "[FAILURE] No contours found.\n";
        return 0.0;
    }

    double max_line_width = 0.0;
    cv::Rect scale_bar_rect;
    std::cout << "[Debug] Found " << contours.size() << " contours. Finding the widest contour that qualifies as a line:\n";

    for (const auto &contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);
        double aspect_ratio = (rect.height == 0) ? 0 : static_cast<double>(rect.width) / rect.height;
        printf("      - Contour: w=%-4d h=%-4d aspect_ratio=%.2f", rect.width, rect.height, aspect_ratio);

        // Initial filter: must look like a line (sufficient aspect ratio) and be within reasonable size range
        if (aspect_ratio > 15.0 && rect.width > 50 && rect.height < 25) {
            printf(" | QUALIFIED");
            // Among all qualified line segments, find the longest (widest) one
            if (rect.width > max_line_width) {
                max_line_width = rect.width;
                scale_bar_rect = rect;
                printf(" -> NEW WIDEST");
            }
        }
        printf("\n");
    }

    // Final check: must have found a qualified scale bar line
    if (scale_bar_rect.width > 0) {
        std::cout << "[SUCCESS] Isolated the scale bar line based on max width among valid lines.\n";
        cv::rectangle(debug_image, scale_bar_rect, cv::Scalar(255, 255, 0), 3);

        double pixel_length = scale_bar_rect.width;
        double um_per_pixel = scale_bar_real_length_um / pixel_length;

        std::cout << "--- Scale Calibration ---\n"
                  << "Scale bar detected. Pixel width: " << pixel_length << "\n"
                  << "Real length: " << scale_bar_real_length_um << " um\n"
                  << "Conversion factor: " << std::fixed << std::setprecision(4) << um_per_pixel << " um/pixel\n"
                  << "-------------------------\n"
                  << std::endl;

        return um_per_pixel;
    }

    std::cout << "[FAILURE] No contour met all qualification criteria.\n";
    std::cout << "--- Scale Calibration ---\n"
              << "WARNING: Scale bar not detected! Measurements will be in pixels.\n"
              << "-------------------------\n"
              << std::endl;

    return 0.0;
}

int main() {
    // Load the image
    cv::Mat src = cv::imread("../images/particle.jpg");
    if (src.empty()) {
        std::cout << "Error: Could not load image!" << std::endl;
        return -1;
    }

    cv::Mat final_result_image = src.clone();
    const double SCALE_BAR_LENGTH_UM = 2000.0;
    double um_per_pixel = getMicronPerPixel(src, SCALE_BAR_LENGTH_UM, final_result_image);
    bool calibration_successful = (um_per_pixel > 0.0);

    // Image preprocessing for particle analysis
    cv::Mat gray, binaryMask, markers;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binaryMask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(binaryMask, binaryMask, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 2);

    // Watershed segmentation
    cv::Mat sure_bg, distTransform, sure_fg, unknown;
    cv::dilate(binaryMask, sure_bg, kernel, cv::Point(-1, -1), 3);
    cv::distanceTransform(binaryMask, distTransform, cv::DIST_L2, 5);

    double minVal_dist, maxVal_dist;
    cv::minMaxLoc(distTransform, &minVal_dist, &maxVal_dist);
    double dynamic_thresh = 0.5 * maxVal_dist;
    cv::threshold(distTransform, sure_fg, dynamic_thresh, 255, cv::THRESH_BINARY);
    sure_fg.convertTo(sure_fg, CV_8U);

    cv::subtract(sure_bg, sure_fg, unknown);
    cv::connectedComponents(sure_fg, markers);
    markers = markers + 1;
    markers.setTo(0, unknown);
    cv::watershed(src, markers);

    std::cout << "\n--- Starting Defect Analysis and Depth Calculation ---\n" << std::endl;

    const double SOLIDITY_THRESHOLD = 0.95;
    double minVal, maxVal;
    cv::minMaxLoc(markers, &minVal, &maxVal);
    int display_id_counter = 1;

    // Analyze each particle
    for (int label = 2; label <= static_cast<int>(maxVal); ++label) {
        cv::Mat particle_mask = (markers == label);
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(particle_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        if (contours.empty()) continue;

        std::sort(contours.begin(), contours.end(), [](const auto &a, const auto &b) {
            return cv::contourArea(a) > cv::contourArea(b);
        });

        std::vector<cv::Point> main_contour = contours[0];
        double area = cv::contourArea(main_contour);
        if (area < 100) continue;

        // Calculate solidity
        std::vector<cv::Point> hull_points;
        cv::convexHull(main_contour, hull_points);
        double hull_area = cv::contourArea(hull_points);
        double solidity = (hull_area > 0) ? area / hull_area : 0;

        cv::Moments M = cv::moments(main_contour);
        cv::Point centroid(static_cast<int>(M.m10 / M.m00), static_cast<int>(M.m01 / M.m00));

        bool is_defective = (solidity < SOLIDITY_THRESHOLD);
        printf("Particle ID: %-3d | Solidity: %.4f | Status: %s\n", 
               display_id_counter, solidity, is_defective ? "Defective" : "OK");

        if (is_defective) {
            // Calculate convexity defects
            std::vector<int> hull_indices;
            cv::convexHull(main_contour, hull_indices, false, false);
            
            if (hull_indices.size() > 3) {
                std::vector<cv::Vec4i> defects;
                cv::convexityDefects(main_contour, hull_indices, defects);
                float max_defect_depth_px = 0;
                cv::Point farthest_pt, start_pt, end_pt;

                for (const cv::Vec4i &v : defects) {
                    float depth = v[3] / 256.0;
                    if (depth > max_defect_depth_px) {
                        max_defect_depth_px = depth;
                        start_pt = main_contour[v[0]];
                        end_pt = main_contour[v[1]];
                        farthest_pt = main_contour[v[2]];
                    }
                }

                if (max_defect_depth_px > 0) {
                    // Visualization: Add convex hull baseline
                    cv::line(final_result_image, start_pt, end_pt, cv::Scalar(255, 0, 255), 2);
                    double depth_in_um = max_defect_depth_px * um_per_pixel;
                    printf("      └─ Max Defect Depth: %.2f pixels (%.2f um)\n", max_defect_depth_px, depth_in_um);

                    cv::Point hull_midpoint = cv::Point((start_pt.x + end_pt.x) / 2, (start_pt.y + end_pt.y) / 2);
                    cv::arrowedLine(final_result_image, hull_midpoint, farthest_pt, cv::Scalar(255, 0, 255), 2);

                    std::stringstream ss;
                    if (calibration_successful) {
                        ss << std::fixed << std::setprecision(1) << depth_in_um << " um";
                    } else {
                        ss << std::fixed << std::setprecision(1) << max_defect_depth_px << " px";
                    }
                    cv::putText(final_result_image, ss.str(), cv::Point(farthest_pt.x + 5, farthest_pt.y), 
                               cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 255), 1);
                }
            }

            // Mark defective particle
            cv::putText(final_result_image, std::to_string(display_id_counter) + "(D)", 
                       cv::Point(centroid.x - 15, centroid.y - 15), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 255), 1);
            cv::drawContours(final_result_image, std::vector<std::vector<cv::Point>>{main_contour}, 
                           -1, cv::Scalar(0, 0, 255), 2);
        } else {
            // Mark normal particle
            cv::putText(final_result_image, std::to_string(display_id_counter), 
                       cv::Point(centroid.x - 10, centroid.y), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0), 1);
        }

        display_id_counter++;
    }

    // Draw watershed boundaries
    for (int i = 0; i < markers.rows; i++) {
        for (int j = 0; j < markers.cols; j++) {
            if (markers.at<int>(i, j) == -1) {
                final_result_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 255, 255);
            }
        }
    }

    // Display final result
    cv::imshow("Particle Analysis Result", final_result_image);

    // Save result image
    if (cv::imwrite("particle_analysis_result.jpg", final_result_image)) {
        std::cout << "\nAnalysis complete. Result saved to 'particle_analysis_result.jpg'" << std::endl;
    } else {
        std::cout << "\nERROR: Could not save the result image." << std::endl;
    }

    cv::waitKey(0);
    return 0;
}
