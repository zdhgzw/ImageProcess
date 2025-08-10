# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

This is a CMake-based C++ project using MinGW64 on Windows.

**Build the project:**
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

**Run the application:**
```bash
./bin/ImageProcessingApp.exe
```

**Clean build:**
```bash
rm -rf build
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

## Dependencies

- **OpenCV 4.12.0**: Installed at `d:/00-infinuo/install` - all image processing operations
- **cvui library**: Located in `third_party/cvui/` - immediate mode GUI framework
- **MinGW64**: Required compiler with C++17 support
- **CMake 3.10+**: Build system

## Architecture Overview

This is an image processing desktop application with a modular architecture:

### Core Components

1. **main.cpp** - Application entry point with exception handling
2. **ImageProcessingApp** - Main GUI application class using cvui
3. **ImageProcessor** - Core image processing logic using OpenCV

### Key Architecture Patterns

**Modal Window System**: All image processing functions use a unified modal window approach:
- Each function opens a dedicated modal window
- Left side: Real-time preview area
- Right side: Parameter controls (sliders, buttons)
- Bottom: Apply/Cancel buttons
- Enum-based function identification (`ModalFunction`, `PreProcessingFunction`)

**Image Processing Pipeline**:
- `originalImage` → `currentImage` → `displayImage`
- Operations work on `currentImage`, results displayed via `displayImage`
- Reset functionality restores from `originalImage`

**UI Layout System**:
- Top: Image display area with automatic scaling
- Bottom: Unified control panel with function buttons
- Modal windows overlay for parameter adjustment

### Functional Categories

The application implements comprehensive image processing features:

1. **Image Loading/Display**: File dialog integration, format support (JPG, PNG, BMP, TIFF)
2. **Color Processing**: Grayscale conversion, HSV color selection, K-means clustering, channel operations
3. **Pre-Processing**: 23 functions in 6 categories (Contrast, Noise Reduction, Blur, Edges, Texture, Correction)

### State Management

- UI state variables for window dimensions and control layout
- Parameter variables for each processing function with automatic reset
- Preview system with real-time parameter feedback
- Processing state management (`isProcessing` flag)

### Critical Implementation Details

- Windows-specific file dialogs via `comdlg32`
- Automatic OpenCV DLL copying to build directory
- RAII memory management with OpenCV Mat objects
- Exception-based error handling
- cvui framework compliance (no unsupported GUI components)

The codebase is designed for extensibility - new image processing functions follow the established modal window pattern and can be easily integrated into the existing enum-based function system.