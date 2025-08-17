# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Build Process (MinGW64 on Windows)
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "MinGW Makefiles"

# Compile
mingw32-make

# Run executable
./bin/ImageProcessingApp.exe
```

### Dependencies
- **OpenCV 4.12.0**: Installed at `d:/00-infinuo/install`
- **cvui library**: Included in `third_party/cvui/`
- **MinGW64**: Required compiler with C++17 support

## Architecture

### Serial Processing Workflow
**Critical**: All image processing follows a strict serial pipeline:
```
Load Image (3-channel color) 
    ↓
Convert to Grayscale (1-channel)
    ↓
Pre-Processing → Segmentation → Clean-Up → Morphology → Measurements
```

- Each operation's output becomes the next operation's input
- Only "Reset Image" returns to original 3-channel state
- All operations after grayscale conversion work on single-channel images

### Modular Design
The application is organized into specialized processing modules:

- **ImageProcessor**: Core image operations and state management
- **PreProcessing**: 23 functions in 6 categories (contrast, noise reduction, blur, edges, texture, correction)
- **Segmentation**: 5 threshold-based segmentation methods
- **Morphology**: 8 morphological operations including feature separation
- **CleanUp**: 2 specialized tools (hole filling, feature rejection)
- **Measurements**: Object counting and quantitative analysis
- **UIComponents**: Modal window system with real-time preview

### UI System
- **cvui-based immediate mode GUI** with unified modal windows
- **Two-stage interface**: Function selection → Parameter adjustment
- **Real-time preview**: All functions provide immediate visual feedback
- **Channel compatibility**: Automatic 3-channel conversion for display only

## Code Patterns

### Function Implementation
All processing modules follow the unified pattern:
```cpp
// In module header
enum class ModuleFunction { FUNCTION_A, FUNCTION_B, ... };

// In module implementation
void ModuleName::applyFunction(cv::Mat& image, ModuleFunction function) {
    // Function-specific processing
}
```

### Modal Window System
Each processing category uses consistent modal window implementation:
- Left side: Real-time preview area
- Right side: Function-specific parameter controls
- Bottom: Apply/Cancel buttons
- Parameters reset to defaults when switching functions

### Memory Management
- RAII principles with OpenCV Mat objects
- Automatic cleanup and resource management
- No manual memory allocation required

## Known Issues & Fixes

### Channel Consistency (CRITICAL)
- **Fixed**: copyTo errors when cvui::image() receives single-channel images
- **Solution**: All display functions automatically convert single-channel to 3-channel for cvui
- **Location**: UIComponents::renderPreviewArea and ImageProcessingApp::renderImageDisplay

### Serial Flow Compliance (CRITICAL)
- **Fixed**: Modules performing independent preprocessing breaking serial flow
- **Solution**: Removed all internal threshold/preprocessing operations from CleanUp and Measurements
- **Ensure**: Each module only performs its core function without modifying input format

## Development Guidelines

### Adding New Functions
1. Follow existing enum-based function selection pattern
2. Implement parameter reset when switching functions
3. Maintain input/output channel consistency
4. Add real-time preview support
5. Follow two-stage modal window interface

### Testing Serial Workflow
Use the test files to verify functionality:
- `serial_workflow_test.md`: Complete pipeline testing
- `grayscale_workflow_test.md`: Grayscale conversion verification
- `test_cleanup_functions.md`: Clean-up module verification

### Common Pitfalls
- **Never** force channel conversion in processing modules
- **Always** preserve input channel format in output
- **Ensure** cvui::image() receives 3-channel images for display
- **Maintain** parameter persistence during modal sessions