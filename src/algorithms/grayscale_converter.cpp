#include "ImageUtils.h"
#include "ArgumentParser.h"
#include <opencv2/opencv.hpp>
#include <iostream>

/**
 * Grayscale conversion algorithm
 * Converts color images to grayscale images
 */
class GrayscaleConverter {
public:
    /**
     * Perform grayscale conversion
     * @param inputImage Input color image
     * @return Converted grayscale image
     */
    static cv::Mat convert(const cv::Mat& inputImage) {
        if (!ImageUtils::isValidImage(inputImage)) {
            std::cerr << "Error: Invalid input image" << std::endl;
            return cv::Mat();
        }

        cv::Mat grayImage;

        // If input image is already grayscale, return a copy directly
        if (inputImage.channels() == 1) {
            std::cout << "Input image is already grayscale, copying directly..." << std::endl;
            inputImage.copyTo(grayImage);
        } else {
            // Convert to grayscale image
            std::cout << "Converting color image to grayscale..." << std::endl;
            cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        }

        std::cout << "Grayscale conversion completed." << std::endl;
        std::cout << "Output image info: " << ImageUtils::getImageInfo(grayImage) << std::endl;

        return grayImage;
    }
    
    /**
     * Display algorithm information
     */
    static void showAlgorithmInfo() {
        std::cout << "=== Grayscale Conversion Algorithm ===" << std::endl;
        std::cout << "Function: Convert color images to grayscale images" << std::endl;
        std::cout << "Method: Using OpenCV's cvtColor function with BGR to GRAY conversion" << std::endl;
        std::cout << "Formula: Gray = 0.299*R + 0.587*G + 0.114*B" << std::endl;
        std::cout << "=======================================" << std::endl << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // Create argument parser
    ArgumentParser parser("grayscale_converter", "Image grayscale conversion tool - Convert color images to grayscale");

    // Add argument definitions
    parser.addArgument("-i", "--input", "Input image file path", true, true);
    parser.addArgument("-o", "--output", "Output image file path", true, true);
    parser.addArgument("-v", "--verbose", "Show detailed information", false, false);

    // Parse command line arguments
    if (!parser.parse(argc, argv)) {
        return 1;
    }

    // Get argument values
    std::string inputPath = parser.getValue("--input");
    std::string outputPath = parser.getValue("--output");
    bool verbose = parser.hasArgument("--verbose");

    if (verbose) {
        GrayscaleConverter::showAlgorithmInfo();
    }

    // Load input image
    cv::Mat inputImage = ImageUtils::loadImage(inputPath);
    if (!ImageUtils::isValidImage(inputImage)) {
        std::cerr << "Program terminated: Unable to load input image." << std::endl;
        return 1;
    }

    // Perform grayscale conversion
    cv::Mat grayImage = GrayscaleConverter::convert(inputImage);
    if (!ImageUtils::isValidImage(grayImage)) {
        std::cerr << "Program terminated: Grayscale conversion failed." << std::endl;
        return 1;
    }

    // Save output image
    if (!ImageUtils::saveImage(grayImage, outputPath)) {
        std::cerr << "Program terminated: Unable to save output image." << std::endl;
        return 1;
    }

    std::cout << std::endl << "Grayscale conversion processing completed!" << std::endl;
    std::cout << "Input file: " << inputPath << std::endl;
    std::cout << "Output file: " << outputPath << std::endl;

    return 0;
}
