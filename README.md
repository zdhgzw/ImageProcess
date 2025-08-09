# Image Processing Application

A comprehensive image processing application built with C++ and OpenCV, featuring an intuitive GUI powered by cvui.

## Features

### 1. Image Loading and Display
- **Load Image**: Load images from file system with support for common formats (JPG, PNG, BMP, TIFF)
- **Reset**: Reset to original image
- **Image Display**: Real-time image preview with automatic scaling

### 2. Color Image Processing
- **Convert to Grayscale**: Convert color images to grayscale
- **Color Select**: Select pixels based on HSV color range with interactive sliders
- **Color Cluster**: K-means clustering for color segmentation (2-8 clusters)
- **Color Deconvolution**: Extract individual color channels (Blue, Green, Red)
- **Channel Operation**: Arithmetic operations on image channels (Add, Subtract, Multiply, Divide)

### 3. Pre-Processing
- **CONTRAST** (parameters reset to defaults each time function is selected):
  - Adjust Contrast (brightness: -100 to +100, contrast: 0.1 to 3.0, defaults: 0.0, 1.0)
  - Histogram Equalization (global vs. adaptive CLAHE with clip limit: 1.0 to 40.0, defaults: global, 2.0)
  - Flatten Background (kernel size: 5 to 51, odd numbers only, default: 15)
- **NOISE-REDUCTION**: Median Filter, Wiener Filter, Non-Local Means Denoising
- **BLUR**: Gaussian Blur, Average Blur, Sum Filter, Grayscale Dilate/Erode
- **EDGES**: StdDev Filter, Entropy Filter, Gradient Filter, Highlight Lines
- **TEXTURE**: Bright Texture, Dark Texture, Advanced Texture, Similarity
- **CORRECTION**: Sharpen, FFT Filter, Grayscale Interpolation, Grayscale Reconstruction

## Build Requirements

- **Compiler**: MinGW64 with C++17 support
- **Build System**: CMake 3.10+
- **Dependencies**: 
  - OpenCV 4.12.0 (installed at `d:/00-infinuo/install`)
  - cvui library (included in `third_party/cvui`)

## Building the Project

1. **Create build directory**:
   ```bash
   mkdir build
   cd build
   ```

2. **Configure with CMake**:
   ```bash
   cmake .. -G "MinGW Makefiles"
   ```

3. **Compile**:
   ```bash
   mingw32-make
   ```

4. **Run**:
   ```bash
   ./bin/ImageProcessingApp.exe
   ```

## Usage

1. **Start the application** - The GUI window opens with an image display area at the top and a unified control panel below
2. **Access functions** - All image processing functions are available through individual buttons in the control panel:
   - **Load Image** - Opens a modal dialog to select and load image files
   - **Reset Image** - Opens a confirmation dialog to reset to the original image
   - **Grayscale** - Opens a modal window with preview for grayscale conversion
   - **Color Select** - Opens a modal window for HSV-based color selection with real-time preview
   - **Clustering** - Opens a modal window for K-means color clustering with parameter controls
   - **Deconvolution** - Opens a modal window for color channel extraction with channel selection
   - **Channel Ops** - Opens a modal window for arithmetic channel operations with operation type and value controls
   - **Pre-Processing** - Opens a modal window with 23 pre-processing functions organized in 6 categories, featuring two-stage interface (function selection → parameter adjustment) with automatic parameter reset

3. **Modal window workflow** - Each function opens a dedicated modal window containing:
   - **Left side**: Real-time preview area showing the effect of current parameters
   - **Right side**: Function-specific parameter controls (sliders, buttons, input fields)
   - **Bottom**: "Apply" button to execute the function and "Cancel" button to close without changes
   - **Update Preview**: Button to refresh the preview with current parameter values

4. **Consistent interaction pattern** - All functions follow the same modal window approach:
   - Click function button → Modal window opens → Adjust parameters → Preview results → Apply or Cancel

5. **Exit** - Press ESC key to close the application

## Project Structure

```
image_process_with_opencv/
├── src/                    # Source files
│   ├── main.cpp           # Application entry point
│   ├── ImageProcessingApp.h/cpp  # Main application class
│   └── ImageProcessor.h/cpp      # Image processing logic
├── third_party/cvui/      # cvui GUI library
├── images/                # Test images
├── build/                 # Build output directory
└── CMakeLists.txt        # CMake configuration
```

## Technical Details

- **Architecture**: Modular design with separate UI and processing classes
- **GUI Framework**: cvui for immediate mode GUI with unified modal window system
- **Image Processing**: OpenCV for all image operations
- **Memory Management**: RAII principles with OpenCV Mat objects
- **Error Handling**: Exception-based error handling with user feedback
- **UI Design**: Unified modal window system providing consistent interaction patterns
- **Preview System**: Real-time preview functionality for all image processing operations
- **Framework Compliance**: Uses only cvui-supported components (buttons, sliders, windows)
- **Modal System**: Enum-based function identification with dedicated modal windows for each operation

## Supported Image Formats

- JPEG (.jpg, .jpeg)
- PNG (.png)
- BMP (.bmp)
- TIFF (.tiff, .tif)

## Controls

- **ESC**: Exit application
- **Mouse**: Interact with GUI elements
- **Function Buttons**: Single-click access to open dedicated modal windows for each operation
- **Modal Windows**: Consistent interface for all functions with preview and parameter controls
- **Sliders**: Drag to adjust parameter values in modal windows
- **Apply/Cancel**: Apply changes or cancel operation in modal windows
- **Update Preview**: Refresh preview with current parameter values
- **File Dialog**: Integrated file selection for image loading

## Notes

- All processing operations work on the current image state
- Use "Reset Image" modal to return to the original loaded image
- Image display automatically scales to fit the display area
- Console output provides feedback on operations and errors
- All functions use the unified modal window system for consistent user experience
- Real-time preview is available for all image processing operations
- Modal windows provide a focused environment for parameter adjustment
- UI design is fully compatible with cvui framework limitations
- Easily extensible - new functions can be added by implementing additional modal windows
- Consistent interaction pattern eliminates the need to learn different interfaces for different functions
