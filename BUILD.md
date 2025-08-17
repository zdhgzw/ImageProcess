# Cross-Platform Build Guide

This document provides instructions for building the Image Processing Application on different operating systems.

## Supported Platforms

- **Windows** (MinGW-w64, MSVC)
- **Linux** (Ubuntu, Debian, CentOS, Arch Linux, etc.)
- **macOS** (experimental support)

## Prerequisites

### Common Requirements

- **CMake** 3.10 or higher
- **C++17** compatible compiler
- **OpenCV** 4.x (recommended) or 3.x

### Platform-Specific Requirements

#### Windows

**Option 1: MinGW-w64 (Recommended)**
- [MSYS2](https://www.msys2.org/) with MinGW-w64
- OpenCV built with MinGW-w64

**Option 2: Visual Studio**
- Visual Studio 2017 or later
- OpenCV built with MSVC

#### Linux

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
sudo apt-get install libopencv-dev
sudo apt-get install libx11-dev libgtk-3-dev  # Optional: for GUI support
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum install gcc-c++ cmake git
sudo yum install opencv-devel
sudo yum install libX11-devel gtk3-devel  # Optional: for GUI support
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake git
sudo pacman -S opencv
sudo pacman -S libx11 gtk3  # Optional: for GUI support
```

#### macOS

```bash
# Install Xcode command line tools
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake opencv
```

## Building the Project

### Quick Build (All Platforms)

#### Linux/macOS:
```bash
# Make build script executable
chmod +x build.sh

# Build release version
./build.sh release

# Build debug version
./build.sh debug

# Clean build
./build.sh clean release
```

#### Windows:
```cmd
REM Build release version
build.bat release

REM Build debug version
build.bat debug

REM Clean build
build.bat clean release
```

### Manual Build

#### 1. Create Build Directory
```bash
mkdir build
cd build
```

#### 2. Configure with CMake

**Linux/macOS:**
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

**Windows (MinGW):**
```cmd
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
```

**Windows (Visual Studio):**
```cmd
cmake -G "Visual Studio 16 2019" -A x64 ..
```

#### 3. Build

**Linux/macOS:**
```bash
make -j$(nproc)
```

**Windows (MinGW):**
```cmd
mingw32-make -j%NUMBER_OF_PROCESSORS%
```

**Windows (Visual Studio):**
```cmd
cmake --build . --config Release
```

## OpenCV Configuration

### Automatic Detection

The CMake configuration will automatically try to find OpenCV in common locations:

**Linux:**
- `/usr/local/lib/cmake/opencv4`
- `/usr/lib/cmake/opencv4`
- `/usr/lib/x86_64-linux-gnu/cmake/opencv4`
- Via pkg-config

**Windows:**
- `d:/00-infinuo/install/x64/mingw/lib` (default path)

### Manual OpenCV Path

If OpenCV is installed in a custom location, set the `OpenCV_DIR` variable:

```bash
cmake -DOpenCV_DIR=/path/to/opencv/lib/cmake/opencv4 ..
```

**Windows example:**
```cmd
cmake -DOpenCV_DIR=C:\opencv\build\x64\vc16\lib -G "Visual Studio 16 2019" ..
```

## Troubleshooting

### Common Issues

#### 1. OpenCV Not Found

**Error:** `Could not find a package configuration file provided by "OpenCV"`

**Solutions:**
- Install OpenCV development packages
- Set `OpenCV_DIR` to the correct path
- Check that OpenCV was built with the same compiler

#### 2. Missing X11 Libraries (Linux)

**Error:** GUI windows don't display properly

**Solution:**
```bash
sudo apt-get install libx11-dev
```

#### 3. DLL Not Found (Windows)

**Error:** Application fails to start due to missing DLLs

**Solutions:**
- Ensure OpenCV DLLs are in the same directory as the executable
- Add OpenCV bin directory to PATH
- The build script automatically copies DLLs to the output directory

#### 4. Compiler Version Mismatch

**Error:** Linking errors or crashes

**Solution:** Ensure OpenCV was built with the same compiler version you're using

### Platform-Specific Notes

#### Linux
- The application requires X11 for GUI display
- GTK3 is optional but recommended for better file dialogs
- Use `ldd bin/ImageProcessingApp` to check library dependencies

#### Windows
- MinGW-w64 is recommended over older MinGW versions
- Ensure all OpenCV DLLs are accessible to the application
- Use Dependency Walker to diagnose DLL issues

#### macOS
- Xcode command line tools are required
- OpenCV from Homebrew should work out of the box
- May require additional permissions for camera access

## Installation

### Linux System-Wide Installation

```bash
cd build
sudo make install
```

This will install:
- Executable to `/usr/local/bin`
- Desktop entry to `/usr/share/applications`

### Creating Desktop Entry (Linux)

```bash
cp build/ImageProcessingApp.desktop ~/.local/share/applications/
```

## Development

### IDE Support

The build system generates `compile_commands.json` for better IDE support:

- **VS Code:** Install C/C++ extension
- **CLion:** Open the project directory
- **Qt Creator:** Open CMakeLists.txt

### Debug Build

For debugging, use the debug build type:

```bash
./build.sh debug
```

This enables:
- Debug symbols (`-g`)
- No optimization (`-O0`)
- Additional compiler warnings

## Contributing

When contributing to the project:

1. Test builds on multiple platforms if possible
2. Follow the existing code style
3. Update this guide if adding new dependencies
4. Ensure cross-platform compatibility

## Support

For build issues:

1. Check this guide first
2. Verify all prerequisites are installed
3. Try a clean build
4. Check the GitHub issues for similar problems
