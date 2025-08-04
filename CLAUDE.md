# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System and Dependencies

This project uses OpenCV for image processing and requires the following dependencies:
- OpenCV (for image processing operations)
- cvui (third-party UI library, located in `third_party/cvui/`)

The project currently lacks a CMakeLists.txt file in the root directory. Build configuration will need to be created to compile the application.

Required system packages (Ubuntu 22.04):
```bash
sudo apt update
sudo apt install -y build-essential cmake libopencv-dev
```

## Code Architecture

The application follows a clean separation of concerns with two main classes:

### Core Components

1. **ImageProcessor** (`src/ImageProcessor.{h,cpp}`)
   - Core image processing functionality
   - Manages three image states: original, current (processed), and display
   - Provides color image processing operations: grayscale conversion, color clustering (K-means), color deconvolution, HSV color selection

2. **ImageProcessingApp** (`src/ImageProcessingApp.{h,cpp}`)
   - Main application interface and UI management
   - Handles keyboard input and window management
   - Integrates with ImageProcessor for processing operations
   - Contains Windows-specific file dialog code (`#ifdef _WIN32`)

3. **main.cpp** (`src/main.cpp`)
   - Application entry point with exception handling
   - Simple initialization and run loop

### Key Features

- **Image Loading**: Supports common formats (jpg, png, bmp, tiff)
- **Color Processing**: 
  - Grayscale conversion
  - K-means color clustering
  - Color deconvolution (channel separation)
  - HSV-based color selection
- **Interactive Controls**: Keyboard-based interface (L=load, R=reset, G=grayscale, C=cluster, 1/2/3=channels, ESC=exit)

### Cross-Platform Considerations

The codebase includes Windows-specific file dialog code but falls back to console input on other platforms. The main image processing logic is platform-independent through OpenCV.

## Development Workflow

Since build system is not yet configured, typical development commands would be:

```bash
# Create build directory
mkdir build && cd build

# Configure build (after CMakeLists.txt is created)
cmake ..

# Build application
make
```

## Code Style Notes

- Chinese comments are used throughout the codebase
- Error messages and console output are in Chinese
- Uses OpenCV Mat objects for image handling
- Exception-based error handling with std::runtime_error
- RAII pattern for resource management