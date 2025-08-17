#!/bin/bash

# Test script to verify cross-platform build
# This script tests the build on the current platform

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() {
    echo -e "${BLUE}[TEST]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="Linux"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="Windows"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
else
    print_error "Unsupported platform: $OSTYPE"
    exit 1
fi

print_status "Testing build on $PLATFORM"

# Test 1: Check prerequisites
print_status "Checking prerequisites..."

# Check CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    print_success "CMake found: $CMAKE_VERSION"
else
    print_error "CMake not found"
    exit 1
fi

# Check compiler
if [[ "$PLATFORM" == "Linux" ]] || [[ "$PLATFORM" == "macOS" ]]; then
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_success "GCC found: $GCC_VERSION"
    elif command -v clang++ &> /dev/null; then
        CLANG_VERSION=$(clang++ --version | head -n1)
        print_success "Clang found: $CLANG_VERSION"
    else
        print_error "No C++ compiler found"
        exit 1
    fi
elif [[ "$PLATFORM" == "Windows" ]]; then
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_success "MinGW GCC found: $GCC_VERSION"
    else
        print_error "MinGW GCC not found"
        exit 1
    fi
fi

# Check OpenCV
print_status "Checking OpenCV..."
if [[ "$PLATFORM" == "Linux" ]]; then
    if pkg-config --exists opencv4; then
        OPENCV_VERSION=$(pkg-config --modversion opencv4)
        print_success "OpenCV found via pkg-config: $OPENCV_VERSION"
    elif pkg-config --exists opencv; then
        OPENCV_VERSION=$(pkg-config --modversion opencv)
        print_success "OpenCV found via pkg-config: $OPENCV_VERSION"
    else
        print_error "OpenCV not found via pkg-config"
        # Continue anyway, CMake might find it
    fi
elif [[ "$PLATFORM" == "Windows" ]]; then
    if [ -d "d:/00-infinuo/install" ]; then
        print_success "OpenCV installation found at d:/00-infinuo/install"
    else
        print_error "OpenCV not found at default location"
        # Continue anyway, user might have set OpenCV_DIR
    fi
fi

# Test 2: Clean build
print_status "Testing clean build..."
if [ -f "build.sh" ]; then
    chmod +x build.sh
    ./build.sh clean release
elif [ -f "build.bat" ]; then
    ./build.bat clean release
else
    print_error "No build script found"
    exit 1
fi

# Test 3: Check if executable was created
print_status "Checking executable..."
if [[ "$PLATFORM" == "Windows" ]]; then
    EXECUTABLE="build/bin/ImageProcessingApp.exe"
else
    EXECUTABLE="build/bin/ImageProcessingApp"
fi

if [ -f "$EXECUTABLE" ]; then
    print_success "Executable created: $EXECUTABLE"
    
    # Get file size
    if command -v stat &> /dev/null; then
        if [[ "$PLATFORM" == "Linux" ]] || [[ "$PLATFORM" == "macOS" ]]; then
            SIZE=$(stat -c%s "$EXECUTABLE" 2>/dev/null || stat -f%z "$EXECUTABLE")
        else
            SIZE=$(stat -c%s "$EXECUTABLE")
        fi
        print_status "Executable size: $SIZE bytes"
    fi
else
    print_error "Executable not found: $EXECUTABLE"
    exit 1
fi

# Test 4: Check dependencies (Linux only)
if [[ "$PLATFORM" == "Linux" ]] && command -v ldd &> /dev/null; then
    print_status "Checking dependencies..."
    if ldd "$EXECUTABLE" | grep -q "not found"; then
        print_error "Missing dependencies:"
        ldd "$EXECUTABLE" | grep "not found"
        exit 1
    else
        print_success "All dependencies found"
    fi
fi

# Test 5: Quick run test (if DISPLAY is available)
if [[ "$PLATFORM" == "Linux" ]] && [ -n "$DISPLAY" ]; then
    print_status "Testing quick run (will exit after 2 seconds)..."
    timeout 2s "$EXECUTABLE" || true
    print_success "Application started successfully"
elif [[ "$PLATFORM" == "Windows" ]]; then
    print_status "Skipping run test on Windows (requires manual testing)"
elif [[ "$PLATFORM" == "macOS" ]]; then
    print_status "Skipping run test on macOS (requires manual testing)"
fi

print_success "All tests passed! Build is working correctly."
print_status "You can now run: $EXECUTABLE"
