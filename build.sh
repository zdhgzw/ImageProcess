#!/bin/bash

# Cross-platform build script for Image Processing App
# Usage: ./build.sh [clean|debug|release]

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="Linux"
    CMAKE_GENERATOR="Unix Makefiles"
    BUILD_TOOL="make"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="Windows"
    CMAKE_GENERATOR="MinGW Makefiles"
    BUILD_TOOL="mingw32-make"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
    CMAKE_GENERATOR="Unix Makefiles"
    BUILD_TOOL="make"
else
    print_error "Unsupported platform: $OSTYPE"
    exit 1
fi

print_status "Building for $PLATFORM"

# Parse command line arguments
BUILD_TYPE="Release"
CLEAN_BUILD=false

for arg in "$@"; do
    case $arg in
        clean)
            CLEAN_BUILD=true
            ;;
        debug)
            BUILD_TYPE="Debug"
            ;;
        release)
            BUILD_TYPE="Release"
            ;;
        *)
            print_warning "Unknown argument: $arg"
            ;;
    esac
done

print_status "Build type: $BUILD_TYPE"

# Project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

# Clean build if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_status "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Check for OpenCV
print_status "Checking for OpenCV..."

if [[ "$PLATFORM" == "Linux" ]]; then
    # Check for OpenCV on Linux
    if pkg-config --exists opencv4; then
        OPENCV_VERSION=$(pkg-config --modversion opencv4)
        print_success "Found OpenCV $OPENCV_VERSION via pkg-config"
    elif [ -d "/usr/local/include/opencv4" ]; then
        print_success "Found OpenCV in /usr/local"
    elif [ -d "/usr/include/opencv4" ]; then
        print_success "Found OpenCV in /usr"
    else
        print_error "OpenCV not found. Please install OpenCV:"
        echo "  Ubuntu/Debian: sudo apt-get install libopencv-dev"
        echo "  CentOS/RHEL:   sudo yum install opencv-devel"
        echo "  Arch Linux:    sudo pacman -S opencv"
        exit 1
    fi
    
    # Check for required system libraries
    if ! pkg-config --exists x11; then
        print_warning "X11 development libraries not found. GUI may not work properly."
        echo "  Install with: sudo apt-get install libx11-dev (Ubuntu/Debian)"
    fi
    
elif [[ "$PLATFORM" == "Windows" ]]; then
    # Check for OpenCV on Windows
    if [ -d "d:/00-infinuo/install" ]; then
        print_success "Found OpenCV installation at d:/00-infinuo/install"
    else
        print_warning "Default OpenCV path not found. Make sure OpenCV is installed and set OpenCV_DIR"
    fi
fi

# Configure with CMake
print_status "Configuring with CMake..."
cmake -G "$CMAKE_GENERATOR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      "$PROJECT_ROOT"

# Build
print_status "Building project..."
$BUILD_TOOL -j$(nproc 2>/dev/null || echo 4)

# Check if build was successful
if [ $? -eq 0 ]; then
    print_success "Build completed successfully!"
    print_status "Executable location: $BUILD_DIR/bin/ImageProcessingApp"
    
    # Platform specific post-build info
    if [[ "$PLATFORM" == "Linux" ]]; then
        print_status "To install system-wide (optional):"
        echo "  sudo make install"
        print_status "To create desktop entry:"
        echo "  cp ImageProcessingApp.desktop ~/.local/share/applications/"
    fi
    
else
    print_error "Build failed!"
    exit 1
fi
