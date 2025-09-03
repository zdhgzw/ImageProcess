# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 响应要求

- **语言**：所有回复均需使用 **中文**
- **格式**：输出的文档要用**Markdown** 格式

## features.md修改

- 实现功能点后，在对应功能后简要描述该功能

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

