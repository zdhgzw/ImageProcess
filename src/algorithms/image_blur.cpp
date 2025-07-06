#include "ImageUtils.h"
#include "ArgumentParser.h"
#include <opencv2/opencv.hpp>
#include <iostream>

/**
 * Image blur algorithm
 * Provides multiple blur effects: Gaussian blur, mean blur, median blur
 */
class ImageBlur {
public:
    enum BlurType {
        GAUSSIAN,   // Gaussian blur
        MEAN,       // Mean blur
        MEDIAN      // Median blur
    };

    /**
     * Apply image blur
     * @param inputImage Input image
     * @param blurType Blur type
     * @param kernelSize Kernel size
     * @param sigmaX X-direction standard deviation (for Gaussian blur only)
     * @param sigmaY Y-direction standard deviation (for Gaussian blur only)
     * @return Blurred image
     */
    static cv::Mat applyBlur(const cv::Mat& inputImage,
                            BlurType blurType = GAUSSIAN,
                            int kernelSize = 15,
                            double sigmaX = 0.0,
                            double sigmaY = 0.0) {
        if (!ImageUtils::isValidImage(inputImage)) {
            std::cerr << "Error: Invalid input image" << std::endl;
            return cv::Mat();
        }

        // Ensure kernel size is odd and greater than 1
        if (kernelSize % 2 == 0) {
            kernelSize++;
        }
        kernelSize = std::max(3, kernelSize);

        cv::Mat blurredImage;

        switch (blurType) {
            case GAUSSIAN:
                std::cout << "Applying Gaussian blur..." << std::endl;
                std::cout << "Parameters: kernel size=" << kernelSize
                          << ", sigmaX=" << sigmaX
                          << ", sigmaY=" << sigmaY << std::endl;
                cv::GaussianBlur(inputImage, blurredImage, 
                               cv::Size(kernelSize, kernelSize), sigmaX, sigmaY);
                break;

            case MEAN:
                std::cout << "Applying mean blur..." << std::endl;
                std::cout << "Parameters: kernel size=" << kernelSize << "x" << kernelSize << std::endl;
                cv::blur(inputImage, blurredImage, cv::Size(kernelSize, kernelSize));
                break;

            case MEDIAN:
                std::cout << "Applying median blur..." << std::endl;
                std::cout << "Parameters: kernel size=" << kernelSize << std::endl;
                cv::medianBlur(inputImage, blurredImage, kernelSize);
                break;

            default:
                std::cerr << "Error: Unknown blur type" << std::endl;
                return cv::Mat();
        }

        std::cout << "Blur processing completed." << std::endl;
        std::cout << "Output image info: " << ImageUtils::getImageInfo(blurredImage) << std::endl;

        return blurredImage;
    }

    /**
     * Convert string to blur type
     */
    static BlurType stringToBlurType(const std::string& typeStr) {
        if (typeStr == "gaussian" || typeStr == "g") {
            return GAUSSIAN;
        } else if (typeStr == "mean" || typeStr == "m") {
            return MEAN;
        } else if (typeStr == "median" || typeStr == "med") {
            return MEDIAN;
        }
        return GAUSSIAN; // Default value
    }
    
    /**
     * Display algorithm information
     */
    static void showAlgorithmInfo() {
        std::cout << "=== Image Blur Algorithm ===" << std::endl;
        std::cout << "Function: Apply different types of blur effects to images" << std::endl;
        std::cout << "Supported blur types:" << std::endl;
        std::cout << "  1. Gaussian blur (gaussian/g): Uses Gaussian kernel for natural blur effect" << std::endl;
        std::cout << "  2. Mean blur (mean/m): Uses mean filter, simple computation" << std::endl;
        std::cout << "  3. Median blur (median/med): Uses median filter, effective for salt-and-pepper noise removal" << std::endl;
        std::cout << "Parameters:" << std::endl;
        std::cout << "  - Kernel size: Size of blur kernel, larger values create stronger blur" << std::endl;
        std::cout << "  - sigmaX/Y: Standard deviation of Gaussian kernel (Gaussian blur only)" << std::endl;
        std::cout << "============================" << std::endl << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // Create argument parser
    ArgumentParser parser("image_blur", "Image blur tool - Apply various blur effects to images");

    // Add argument definitions
    parser.addArgument("-i", "--input", "Input image file path", true, true);
    parser.addArgument("-o", "--output", "Output image file path", true, true);
    parser.addArgument("-t", "--type", "Blur type: gaussian(g)/mean(m)/median(med) (default: gaussian)", false, true);
    parser.addArgument("-k", "--kernel", "Kernel size (default: 15)", false, true);
    parser.addArgument("-x", "--sigmax", "X-direction standard deviation (Gaussian blur only, default: 0-auto)", false, true);
    parser.addArgument("-y", "--sigmay", "Y-direction standard deviation (Gaussian blur only, default: 0-auto)", false, true);
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
    std::string typeStr = parser.hasArgument("--type") ? parser.getValue("--type") : "gaussian";
    ImageBlur::BlurType blurType = ImageBlur::stringToBlurType(typeStr);

    int kernelSize = 15;
    if (parser.hasArgument("--kernel")) {
        kernelSize = std::stoi(parser.getValue("--kernel"));
        kernelSize = std::max(3, kernelSize);
    }

    double sigmaX = 0.0;
    double sigmaY = 0.0;
    if (parser.hasArgument("--sigmax")) {
        sigmaX = std::stod(parser.getValue("--sigmax"));
    }
    if (parser.hasArgument("--sigmay")) {
        sigmaY = std::stod(parser.getValue("--sigmay"));
    }

    if (verbose) {
        ImageBlur::showAlgorithmInfo();
    }

    // Load input image
    cv::Mat inputImage = ImageUtils::loadImage(inputPath);
    if (!ImageUtils::isValidImage(inputImage)) {
        std::cerr << "Program terminated: Unable to load input image." << std::endl;
        return 1;
    }

    // Perform blur processing
    cv::Mat blurredImage = ImageBlur::applyBlur(inputImage, blurType, kernelSize, sigmaX, sigmaY);
    if (!ImageUtils::isValidImage(blurredImage)) {
        std::cerr << "Program terminated: Blur processing failed." << std::endl;
        return 1;
    }

    // Save output image
    if (!ImageUtils::saveImage(blurredImage, outputPath)) {
        std::cerr << "Program terminated: Unable to save output image." << std::endl;
        return 1;
    }

    std::cout << std::endl << "Image blur processing completed!" << std::endl;
    std::cout << "Input file: " << inputPath << std::endl;
    std::cout << "Output file: " << outputPath << std::endl;
    std::cout << "Blur type: " << typeStr << std::endl;
    std::cout << "Kernel size: " << kernelSize << std::endl;
    if (blurType == ImageBlur::GAUSSIAN) {
        std::cout << "Sigma parameters: X=" << sigmaX << ", Y=" << sigmaY << std::endl;
    }
    
    return 0;
}
