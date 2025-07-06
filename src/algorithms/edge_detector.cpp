#include "ImageUtils.h"
#include "ArgumentParser.h"
#include <opencv2/opencv.hpp>
#include <iostream>

/**
 * Canny edge detection algorithm
 * Uses Canny algorithm to detect edges in images
 */
class EdgeDetector {
public:
    /**
     * Perform Canny edge detection
     * @param inputImage Input image
     * @param lowThreshold Low threshold
     * @param highThreshold High threshold
     * @param kernelSize Sobel kernel size
     * @return Edge detection result image
     */
    static cv::Mat detectEdges(const cv::Mat& inputImage,
                              double lowThreshold = 50.0,
                              double highThreshold = 150.0,
                              int kernelSize = 3) {
        if (!ImageUtils::isValidImage(inputImage)) {
            std::cerr << "Error: Invalid input image" << std::endl;
            return cv::Mat();
        }

        cv::Mat grayImage, edges;

        // Convert to grayscale image (if needed)
        if (inputImage.channels() == 3) {
            std::cout << "Converting color image to grayscale..." << std::endl;
            cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        } else {
            grayImage = inputImage.clone();
        }

        // Apply Gaussian blur to reduce noise
        cv::Mat blurred;
        cv::GaussianBlur(grayImage, blurred, cv::Size(5, 5), 1.4);
        std::cout << "Applying Gaussian blur to reduce noise..." << std::endl;

        // Perform Canny edge detection
        std::cout << "Performing Canny edge detection..." << std::endl;
        std::cout << "Parameters: low threshold=" << lowThreshold
                  << ", high threshold=" << highThreshold
                  << ", kernel size=" << kernelSize << std::endl;

        cv::Canny(blurred, edges, lowThreshold, highThreshold, kernelSize);

        std::cout << "Edge detection completed." << std::endl;
        std::cout << "Output image info: " << ImageUtils::getImageInfo(edges) << std::endl;
        
        return edges;
    }
    
    /**
     * Display algorithm information
     */
    static void showAlgorithmInfo() {
        std::cout << "=== Canny Edge Detection Algorithm ===" << std::endl;
        std::cout << "Function: Detect edge features in images" << std::endl;
        std::cout << "Method: Canny edge detection algorithm" << std::endl;
        std::cout << "Steps:" << std::endl;
        std::cout << "  1. Gaussian blur noise reduction" << std::endl;
        std::cout << "  2. Calculate gradient intensity and direction" << std::endl;
        std::cout << "  3. Non-maximum suppression" << std::endl;
        std::cout << "  4. Double threshold detection and edge linking" << std::endl;
        std::cout << "Parameters:" << std::endl;
        std::cout << "  - Low threshold: Threshold for weak edges" << std::endl;
        std::cout << "  - High threshold: Threshold for strong edges" << std::endl;
        std::cout << "  - Kernel size: Sobel operator kernel size" << std::endl;
        std::cout << "=======================================" << std::endl << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // Create argument parser
    ArgumentParser parser("edge_detector", "Canny edge detection tool - Detect edge features in images");

    // Add argument definitions
    parser.addArgument("-i", "--input", "Input image file path", true, true);
    parser.addArgument("-o", "--output", "Output image file path", true, true);
    parser.addArgument("-l", "--low", "Low threshold (default: 50)", false, true);
    parser.addArgument("-h", "--high", "High threshold (default: 150)", false, true);
    parser.addArgument("-k", "--kernel", "Sobel kernel size (default: 3)", false, true);
    parser.addArgument("-v", "--verbose", "Show detailed information", false, false);

    // Parse command line arguments
    if (!parser.parse(argc, argv)) {
        return 1;
    }

    // Get argument values
    std::string inputPath = parser.getValue("--input");
    std::string outputPath = parser.getValue("--output");
    bool verbose = parser.hasArgument("--verbose");

    // Get algorithm parameters
    double lowThreshold = 50.0;
    double highThreshold = 150.0;
    int kernelSize = 3;

    if (parser.hasArgument("--low")) {
        lowThreshold = std::stod(parser.getValue("--low"));
    }
    if (parser.hasArgument("--high")) {
        highThreshold = std::stod(parser.getValue("--high"));
    }
    if (parser.hasArgument("--kernel")) {
        kernelSize = std::stoi(parser.getValue("--kernel"));
        // Ensure kernel size is odd
        if (kernelSize % 2 == 0) {
            kernelSize++;
        }
        // Limit kernel size range
        kernelSize = std::max(3, std::min(7, kernelSize));
    }

    if (verbose) {
        EdgeDetector::showAlgorithmInfo();
    }

    // Load input image
    cv::Mat inputImage = ImageUtils::loadImage(inputPath);
    if (!ImageUtils::isValidImage(inputImage)) {
        std::cerr << "Program terminated: Unable to load input image." << std::endl;
        return 1;
    }

    // Perform edge detection
    cv::Mat edgeImage = EdgeDetector::detectEdges(inputImage, lowThreshold, highThreshold, kernelSize);
    if (!ImageUtils::isValidImage(edgeImage)) {
        std::cerr << "Program terminated: Edge detection failed." << std::endl;
        return 1;
    }

    // Save output image
    if (!ImageUtils::saveImage(edgeImage, outputPath)) {
        std::cerr << "Program terminated: Unable to save output image." << std::endl;
        return 1;
    }

    std::cout << std::endl << "Edge detection processing completed!" << std::endl;
    std::cout << "Input file: " << inputPath << std::endl;
    std::cout << "Output file: " << outputPath << std::endl;
    std::cout << "Parameters used: low threshold=" << lowThreshold
              << ", high threshold=" << highThreshold
              << ", kernel size=" << kernelSize << std::endl;
    
    return 0;
}
