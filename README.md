# OpenCV Image Processing Project

A comprehensive C++ image processing framework built with OpenCV, featuring modular architecture where each algorithm is compiled as an independent executable program.

## 🎉 Project Status: Successfully Compiled!

Your C++ OpenCV image processing project has been successfully compiled and all core functionality is implemented and ready to use.

## ✨ Features

- 🔧 **MinGW64 Compiler** with CMake build system
- 📁 **Modular Architecture** - easy to extend and maintain
- 🖼️ **Multiple Image Formats** - supports JPEG, PNG, BMP, and more
- ⚡ **Independent Executables** - each algorithm as standalone program
- 🎯 **Unified CLI Interface** - consistent command-line argument processing
- 📖 **Comprehensive Documentation** - detailed usage instructions and help
- ✅ **Error Handling** - robust input validation and error reporting
- 🌐 **Cross-Platform** - Windows MinGW environment support

## 📁 Project Structure

```
image_process_with_opencv/
├── CMakeLists.txt              # Main CMake configuration
├── README.md                   # Project documentation
├── build/bin/                  # ✅ Compiled executable files
│   ├── grayscale_converter.exe # Grayscale conversion tool
│   ├── edge_detector.exe       # Edge detection tool
│   └── image_blur.exe          # Image blur tool
├── docs/                       # Documentation directory
├── examples/                   # Sample images and scripts
├── include/                    # Header files
│   ├── ImageUtils.h            # Image utility class
│   └── ArgumentParser.h        # Command-line argument parser
└── src/                        # Source code directory
    ├── utils/                  # Utility class implementations
    │   ├── ImageUtils.cpp
    │   └── ArgumentParser.cpp
    └── algorithms/             # Algorithm implementations
        ├── grayscale_converter.cpp  # Grayscale conversion
        ├── edge_detector.cpp        # Edge detection
        └── image_blur.cpp           # Image blur
```

## 🔧 Build Information

- **Compiler**: MinGW64 GCC
- **Build System**: CMake + MinGW Makefiles
- **OpenCV Version**: 4.12.0
- **OpenCV Path**: D:/00-infinuo/install
- **Build Status**: ✅ Successfully Compiled

## 📋 Requirements

### Required Software
- **MinGW64**: C++ compiler
- **CMake**: Build system (version >= 3.16)
- **OpenCV**: Computer vision library (version >= 4.0)

### Windows Environment Setup

1. **Set Opencv Environment Variables**
   - Add MinGW64 bin directory to PATH
   - Ensure CMake and OpenCV can be found
   - **Important**: Set OpenCV DLL path for runtime:
     ```bash
     # In Git Bash:
     export PATH="/d/00-infinuo/install/x64/mingw/bin:$PATH"

     # In Windows CMD:
     set PATH=D:\00-infinuo\install\x64\mingw\bin;%PATH%
     ```

## 🚀 Building the Project

### Using CMake

1. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

2. **Configure project**
   ```bash
   cmake .. -G "MinGW Makefiles"
   ```

3. **Build project**
   ```bash
   cmake --build .
   # or use mingw32-make directly:
   mingw32-make -j4
   ```

4. **After successful build, executables are located in `build/bin/` directory**

### Quick Build Commands

```bash
# One-line build command
mkdir build && cd build && cmake .. -G "MinGW Makefiles" && mingw32-make -j4

# Clean and rebuild
mingw32-make clean && mingw32-make -j4
```

## 📖 Usage Guide

### Quick Start

1. **Test program functionality**
   ```bash
   # View help information
   ./build/bin/grayscale_converter.exe --help
   ./build/bin/edge_detector.exe --help
   ./build/bin/image_blur.exe --help
   ```

2. **Process images**
   ```bash
   # Grayscale conversion
   ./build/bin/grayscale_converter.exe -i input.jpg -o output_gray.jpg

   # Edge detection (Canny)
   ./build/bin/edge_detector.exe -i input.jpg -o edges.jpg --low-threshold 50 --high-threshold 150

   # Image blur
   ./build/bin/image_blur.exe -i input.jpg -o blur.jpg --type gaussian --kernel-size 15
   ```

### 1. Grayscale Converter (grayscale_converter.exe)

Converts color images to grayscale using OpenCV's cvtColor function.

**Basic Usage:**
```bash
./build/bin/grayscale_converter.exe -i input.jpg -o output_gray.jpg
```

**Parameters:**
- `-i, --input`: Input image file path (required)
- `-o, --output`: Output image file path (required)
- `-v, --verbose`: Display detailed information
- `-h, --help`: Show help information

**Examples:**
```bash
# Basic conversion
./build/bin/grayscale_converter.exe -i examples/sample.jpg -o output/gray.jpg

# With verbose output
./build/bin/grayscale_converter.exe -i examples/sample.jpg -o output/gray.jpg -v
```

### 2. Edge Detector (edge_detector.exe)

Detects edges in images using the Canny algorithm with Gaussian blur preprocessing.

**Basic Usage:**
```bash
./build/bin/edge_detector.exe -i input.jpg -o edges.jpg
```

**Parameters:**
- `-i, --input`: Input image file path (required)
- `-o, --output`: Output image file path (required)
- `--low-threshold`: Canny low threshold (default: 50)
- `--high-threshold`: Canny high threshold (default: 150)
- `-v, --verbose`: Display detailed information
- `-h, --help`: Show help information

**Examples:**
```bash
# Using default parameters
./build/bin/edge_detector.exe -i examples/sample.jpg -o output/edges.jpg

# Custom thresholds
./build/bin/edge_detector.exe -i examples/sample.jpg -o output/edges.jpg --low-threshold 30 --high-threshold 100

# With verbose output
./build/bin/edge_detector.exe -i examples/sample.jpg -o output/edges.jpg -v
```

### 3. Image Blur (image_blur.exe)

Applies different types of blur effects to images including Gaussian, mean, and median blur.

**Basic Usage:**
```bash
./build/bin/image_blur.exe -i input.jpg -o blurred.jpg
```

**Parameters:**
- `-i, --input`: Input image file path (required)
- `-o, --output`: Output image file path (required)
- `--type`: Blur type - gaussian/mean/median (default: gaussian)
- `--kernel-size`: Kernel size (default: 15, must be odd)
- `--sigma-x`: X-direction standard deviation (Gaussian blur only)
- `--sigma-y`: Y-direction standard deviation (Gaussian blur only)
- `-v, --verbose`: Display detailed information
- `-h, --help`: Show help information

**Examples:**
```bash
# Gaussian blur (default)
./build/bin/image_blur.exe -i examples/sample.jpg -o output/blur_gaussian.jpg

# Mean blur
./build/bin/image_blur.exe -i examples/sample.jpg -o output/blur_mean.jpg --type mean

# Median blur
./build/bin/image_blur.exe -i examples/sample.jpg -o output/blur_median.jpg --type median

# Custom kernel size
./build/bin/image_blur.exe -i examples/sample.jpg -o output/blur_custom.jpg --kernel-size 25

# Custom Gaussian parameters
./build/bin/image_blur.exe -i examples/sample.jpg -o output/blur_gauss.jpg --type gaussian --kernel-size 21 --sigma-x 8.0 --sigma-y 8.0
```

## 🛠️ Development Information

### Core Classes and Components
- **ImageUtils**: Image I/O operations and validation
- **ArgumentParser**: Command-line argument parsing
- **Modular Design**: Each algorithm as independent executable

### OpenCV Configuration
- **Installation Path**: D:/00-infinuo/install
- **Version**: 4.12.0
- **Architecture**: x64
- **Runtime**: mingw
- **Include Path**: D:/00-infinuo/install/include
- **Library Path**: D:/00-infinuo/install/x64/mingw/lib

## 🔧 Adding New Algorithms

To add new image processing algorithms, follow these steps:

1. **Create a new `.cpp` file in `src/algorithms/` directory**
2. **Include necessary headers**:
   ```cpp
   #include "ImageUtils.h"
   #include "ArgumentParser.h"
   #include <opencv2/opencv.hpp>
   ```
3. **Implement algorithm class and main function**
4. **Add new executable in `CMakeLists.txt`**:
   ```cmake
   set(ALGORITHMS
       grayscale_converter
       edge_detector
       image_blur
       your_new_algorithm  # Add here
   )
   ```

## ⚠️ Runtime Requirements & Troubleshooting

### DLL Dependencies
If programs show "DLL not found" errors, use one of these solutions:

#### Solution 1: Set Environment Variable (Recommended)
```bash
# In Git Bash:
export PATH="/d/00-infinuo/install/x64/mingw/bin:$PATH"

# In Windows CMD:
set PATH=D:\00-infinuo\install\x64\mingw\bin;%PATH%
```

#### Solution 2: Static Linking
```bash
cd build
cmake .. -G "MinGW Makefiles" -DOpenCV_DIR="/d/00-infinuo/install" -DBUILD_SHARED_LIBS=OFF
mingw32-make clean && mingw32-make -j4
```

### Common Issues

1. **OpenCV Not Found**
   - Ensure OpenCV is correctly installed
   - Check environment variable settings
   - Try specifying OpenCV path: `cmake .. -DOpenCV_DIR=/path/to/opencv`

2. **Compilation Errors**
   - Ensure using MinGW64 compiler
   - Check C++ standard support (requires C++17)
   - Verify all dependency libraries are installed

3. **Runtime Errors**
   - Check input file path is correct
   - Ensure output directory exists or has write permissions
   - Verify image file format is supported

## 📝 Usage Recommendations

1. **Prepare test images**: Place test images in `examples/sample_images/` directory
2. **Read documentation**: Check `docs/` directory for detailed documentation
3. **Custom development**: Add new image processing algorithms based on existing framework

## 🎯 Project Highlights

✅ **Modular Architecture** - Each algorithm as independent executable
✅ **Command-Line Interface** - Parameterized configuration support
✅ **Error Handling** - Comprehensive input validation and error reporting
✅ **Cross-Platform Compatible** - Windows MinGW environment support
✅ **Complete Documentation** - Detailed usage instructions and API documentation

## 📄 License

This project is licensed under the MIT License. See LICENSE file for details.

## 🤝 Contributing

Welcome to submit issue reports and feature requests! If you want to contribute code:

1. Fork this project
2. Create feature branch
3. Commit changes
4. Create Pull Request

## 📞 Contact

For questions or suggestions, please contact through:
- Submit GitHub Issues
- Email project maintainers

---

**🎉 Congratulations! Your OpenCV image processing project has been successfully completed and is ready to use!**

If you have any questions, please refer to the project documentation or check the build logs.
