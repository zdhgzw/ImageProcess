#include "ImageUtils.h"
#include <iostream>
#include <sys/stat.h>
#include <direct.h>  // for _mkdir on Windows

cv::Mat ImageUtils::loadImage(const std::string& imagePath) {
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    
    if (image.empty()) {
        std::cerr << "Error: Unable to read image file '" << imagePath << "'" << std::endl;
        std::cerr << "Please check if the file path is correct and the file format is supported." << std::endl;
        return cv::Mat();
    }

    std::cout << "Successfully loaded image: " << imagePath << std::endl;
    std::cout << getImageInfo(image) << std::endl;
    
    return image;
}

bool ImageUtils::saveImage(const cv::Mat& image, const std::string& outputPath) {
    if (!isValidImage(image)) {
        std::cerr << "Error: Invalid image data, cannot save." << std::endl;
        return false;
    }

    // Create output directory
    if (!createOutputDirectory(outputPath)) {
        return false;
    }

    bool success = cv::imwrite(outputPath, image);

    if (success) {
        std::cout << "Image successfully saved to: " << outputPath << std::endl;
    } else {
        std::cerr << "Error: Unable to save image to '" << outputPath << "'" << std::endl;
        std::cerr << "Please check if the output path is valid and you have write permissions." << std::endl;
    }
    
    return success;
}

bool ImageUtils::isValidImage(const cv::Mat& image) {
    return !image.empty() && image.rows > 0 && image.cols > 0;
}

std::string ImageUtils::getImageInfo(const cv::Mat& image) {
    if (!isValidImage(image)) {
        return "Invalid image";
    }

    std::string info = "Image info: ";
    info += std::to_string(image.cols) + "x" + std::to_string(image.rows);
    info += " pixels, " + std::to_string(image.channels()) + " channels";

    switch (image.type()) {
        case CV_8UC1: info += " (8-bit grayscale)"; break;
        case CV_8UC3: info += " (8-bit color)"; break;
        case CV_8UC4: info += " (8-bit RGBA)"; break;
        case CV_16UC1: info += " (16-bit grayscale)"; break;
        case CV_16UC3: info += " (16-bit color)"; break;
        case CV_32FC1: info += " (32-bit float grayscale)"; break;
        case CV_32FC3: info += " (32-bit float color)"; break;
        default: info += " (other format)"; break;
    }

    return info;
}

bool ImageUtils::createOutputDirectory(const std::string& outputPath) {
    // Find the last path separator
    size_t lastSlash = outputPath.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        // No path separator, file is in current directory
        return true;
    }

    std::string dirPath = outputPath.substr(0, lastSlash);

    // Check if directory exists
    struct stat info;
    if (stat(dirPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
        // Directory already exists
        return true;
    }

    // Try to create directory
#ifdef _WIN32
    if (_mkdir(dirPath.c_str()) == 0) {
#else
    if (mkdir(dirPath.c_str(), 0755) == 0) {
#endif
        std::cout << "Created output directory: " << dirPath << std::endl;
        return true;
    } else {
        std::cerr << "Error: Unable to create output directory: " << dirPath << std::endl;
        return false;
    }
}
