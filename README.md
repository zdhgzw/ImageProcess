# Image Processing Application

A comprehensive image processing application built with C++ and OpenCV, featuring an intuitive GUI powered by cvui. This modular application provides professional-grade image processing capabilities with real-time preview and parameter adjustment.

## Features

### 1. Image Loading and Display
- **Load Image**: Load images from file system with support for common formats (JPG, PNG, BMP, TIFF)
- **Reset**: Reset to original image
- **Image Display**: Real-time image preview with automatic scaling

### 2. Color Image Processing
- **Convert to Grayscale**: Convert color images to grayscale
- **Color Select**: Select pixels based on HSV color range with interactive sliders
- **Color Cluster**: K-means clustering for color segmentation (2-16 clusters)
- **Color Deconvolution**: Extract individual color channels (Blue, Green, Red)
- **Channel Operation**: Arithmetic operations on image channels (Add, Subtract, Multiply, Divide)

### 3. Pre-Processing (23 Functions in 6 Categories)
#### 3.1 CONTRAST (对比度调节)
- **Adjust Contrast**: Brightness (-100 to 100) and contrast (0.1 to 3.0) adjustment
- **Histogram Equalization**: Global and adaptive (CLAHE) histogram equalization
- **Flatten Background**: Remove brightness gradients using Gaussian blur

#### 3.2 NOISE-REDUCTION (降噪处理)
- **Median Filter**: Remove salt-and-pepper noise
- **Wiener Filter**: Advanced noise reduction with frequency domain filtering
- **Non-Local Means**: Preserve textures while reducing noise

#### 3.3 BLUR (模糊处理)
- **Gaussian Blur**: Standard Gaussian smoothing with sigma controls
- **Average Blur**: Simple averaging filter
- **Sum Filter**: Summation-based smoothing
- **Grayscale Dilate/Erode**: Morphological operations on grayscale images

#### 3.4 EDGES (突出边界)
- **StdDev Filter**: Standard deviation-based edge enhancement
- **Entropy Filter**: Information theory-based edge detection
- **Gradient Filter**: Gradient magnitude calculation
- **Highlight Lines**: Linear feature enhancement

#### 3.5 TEXTURE (突出纹理特征)
- **Bright/Dark Texture**: Enhance bright or dark textural features
- **Advanced Texture**: Complex texture analysis
- **Similarity**: Texture similarity mapping

#### 3.6 CORRECTION (图像修正)
- **Sharpen**: Image sharpening with adjustable strength
- **FFT Filter**: Frequency domain filtering
- **Grayscale Interpolation/Reconstruction**: Advanced image restoration

### 4. Segmentation (5 Threshold Methods)
- **Basic Threshold**: Simple binary thresholding with value and type controls
- **Range Threshold**: Threshold within specified value range
- **Adaptive Threshold**: Local adaptive thresholding with method and parameter controls
- **EM Threshold**: Expectation-Maximization based thresholding
- **Local Threshold**: Block-based local thresholding

### 5. Morphology (8 Operations)
#### 5.1 Basic Morphological Operations
- **Dilate**: Expand white regions
- **Erode**: Shrink white regions
- **Opening**: Remove small noise (erode + dilate)
- **Closing**: Fill small gaps (dilate + erode)
- **Gradient**: Morphological gradient (dilate - erode)
- **Top Hat**: Highlight bright details
- **Black Hat**: Highlight dark details

#### 5.2 Advanced Operations
- **Separate Features**: Advanced feature separation using edge detection and morphological operations

### 6. Clean-Up (2 Specialized Tools)
- **Fill All Holes**: Automatically fill holes with size and method controls
  - Minimum hole size parameter (1-500)
  - Fill methods: Simple Fill, Morphological Fill, Flood Fill
- **Reject Features**: Remove features based on size and shape criteria
  - Size range controls (min: 1-1000, max: 100-5000)
  - Rejection methods: Size-based, Area+Morphology, Contour-based

### 7. Measurements (Quantitative Analysis)
- **Count Objects**: Object detection and counting with detailed statistics
  - Size range controls (min: 1-1000, max: 100-50000)
  - Detection sensitivity adjustment (0.1-1.0)
  - Results: Object count, total area, average size, size range
  - Visualization: Numbered objects with colored contours and statistical overlay

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
- **Platform**: Windows (with Linux/Mac support possible)

## Feature Summary

| Category | Functions | Description |
|----------|-----------|-------------|
| **Image Management** | 2 | Load images, reset to original |
| **Color Processing** | 5 | Grayscale, HSV selection, clustering, deconvolution, channel ops |
| **Pre-Processing** | 23 | 6 categories: contrast, noise reduction, blur, edges, texture, correction |
| **Segmentation** | 5 | Threshold-based methods for object isolation |
| **Morphology** | 8 | Basic and advanced morphological operations |
| **Clean-Up** | 2 | Hole filling and feature rejection tools |
| **Measurements** | 1 | Object counting and quantitative analysis |
| **Total Functions** | **46** | Complete image processing pipeline |

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

1. **Start the application** - The GUI window opens with an image display area and a unified control panel
2. **Load an image** - Click "Load Image" to select and load image files
3. **Access processing functions** - All image processing functions are available through individual buttons:
   - **Load Image** - File dialog for image selection
   - **Convert to Grayscale** - Simple grayscale conversion
   - **Color Select** - HSV-based color selection with real-time preview
   - **Color Cluster** - K-means color clustering with parameter controls
   - **Color Deconvolution** - Color channel extraction
   - **Channel Operation** - Arithmetic operations on image channels
   - **Pre-Processing** - 23 advanced pre-processing functions in 6 categories
   - **Segmentation** - 5 threshold-based segmentation methods
   - **Morphology** - 8 morphological operations including feature separation
   - **Clean-Up** - Specialized hole filling and feature rejection tools
   - **Measurements** - Object counting and quantitative analysis
   - **Reset Image** - Return to original image

4. **Modal window workflow** - Each function opens a dedicated modal window containing:
   - **Left side**: Real-time preview area showing the effect of current parameters
   - **Right side**: Function-specific parameter controls (sliders, buttons, dropdowns)
   - **Bottom**: "Apply" button to execute the function and "Cancel" button to close without changes
   - **Two-stage interface**: Function selection → Parameter adjustment with automatic parameter reset

5. **Advanced features**:
   - **Real-time preview**: All functions provide immediate visual feedback
   - **Parameter persistence**: Settings are maintained during the session
   - **Detailed results**: Measurements provide comprehensive statistics
   - **Visualization**: Object detection includes numbered overlays and colored contours

6. **Exit** - Press ESC key to close the application

## Project Structure

```
image_process_with_opencv/
├── include/                    # Header files
│   ├── ImageProcessingApp.h   # Main application class
│   ├── ImageProcessor.h       # Core image processing
│   ├── PreProcessing.h        # Pre-processing algorithms (23 functions)
│   ├── Segmentation.h         # Segmentation methods (5 functions)
│   ├── Morphology.h           # Morphological operations (8 functions)
│   ├── CleanUp.h              # Clean-up tools (2 functions)
│   ├── Measurements.h         # Measurement and analysis
│   └── UIComponents.h         # UI component system
├── src/                       # Source files
│   ├── main.cpp              # Application entry point
│   ├── ImageProcessingApp.cpp # Main application implementation
│   ├── ImageProcessor.cpp     # Core processing logic
│   ├── PreProcessing.cpp      # Pre-processing implementations
│   ├── Segmentation.cpp       # Segmentation implementations
│   ├── Morphology.cpp         # Morphological implementations
│   ├── CleanUp.cpp            # Clean-up implementations
│   ├── Measurements.cpp       # Measurement implementations
│   └── UIComponents.cpp       # UI system implementation
├── third_party/cvui/          # cvui GUI library
├── images/                    # Test images
├── build/                     # Build output directory
├── .spec-workflow/            # Specification documents
└── CMakeLists.txt            # CMake configuration
```

## Technical Details

### Architecture
- **Modular Design**: Separate modules for each processing category (PreProcessing, Segmentation, Morphology, CleanUp, Measurements)
- **Single Responsibility**: Each class handles one specific domain of image processing
- **Clean Interfaces**: Unified `applyFunction()` pattern across all modules
- **UI Separation**: Complete separation between processing logic and user interface

### Technology Stack
- **Language**: C++17 with modern features
- **GUI Framework**: cvui for immediate mode GUI with unified modal window system
- **Image Processing**: OpenCV 4.12.0 for all image operations
- **Build System**: CMake with MinGW64 compiler support
- **Memory Management**: RAII principles with OpenCV Mat objects

### Design Patterns
- **Strategy Pattern**: Modular algorithm selection and execution
- **Template Method**: Consistent parameter handling across all functions
- **Observer Pattern**: Real-time preview updates
- **Factory Pattern**: Dynamic function creation based on enum selection

### Performance Features
- **Real-time Preview**: Sub-100ms parameter change response
- **Memory Optimization**: Efficient image copying and processing
- **Lazy Evaluation**: Preview generation only when parameters change
- **Resource Management**: Automatic cleanup and memory management

## Supported Image Formats

- JPEG (.jpg, .jpeg)
- PNG (.png)
- BMP (.bmp)
- TIFF (.tiff, .tif)

## Controls

- **ESC**: Exit application
- **Mouse**: Interact with GUI elements
- **Function Buttons**: Single-click access to open dedicated modal windows
- **Modal Windows**: Consistent interface with preview and parameter controls
- **Sliders**: Real-time parameter adjustment with immediate preview updates
- **Dropdown Menus**: Method and option selection for advanced functions
- **Apply/Cancel**: Apply changes or cancel operation
- **Update Preview**: Manual preview refresh when needed
- **File Dialog**: Integrated file selection for image loading
- **Two-Stage Interface**: Function selection → Parameter adjustment for complex modules

## Development Features

### Extensibility
- **Modular Architecture**: Easy to add new processing modules
- **Consistent Patterns**: All modules follow the same design pattern
- **Clean Interfaces**: Well-defined APIs for each processing category
- **Template System**: Standardized parameter handling and UI generation

### Debugging and Monitoring
- **Console Output**: Detailed feedback on operations and errors
- **Parameter Tracking**: Real-time parameter value display
- **Performance Monitoring**: Processing time and memory usage feedback
- **Error Handling**: Graceful error recovery with user notifications

### Code Quality
- **C++17 Standards**: Modern C++ features and best practices
- **Memory Safety**: RAII and smart pointer usage
- **Exception Safety**: Comprehensive error handling
- **Documentation**: Extensive inline documentation and comments

## Notes

- All processing operations work on the current image state
- Use "Reset Image" to return to the original loaded image
- Image display automatically scales to fit the display area
- Real-time preview is available for all image processing operations
- Modal windows provide a focused environment for parameter adjustment
- Consistent interaction pattern across all functions
- Easily extensible architecture for adding new processing capabilities
- Professional-grade algorithms suitable for research and industrial applications
