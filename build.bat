@echo off
REM Cross-platform build script for Image Processing App (Windows)
REM Usage: build.bat [clean|debug|release]

setlocal enabledelayedexpansion

REM Default settings
set BUILD_TYPE=Release
set CLEAN_BUILD=false
set CMAKE_GENERATOR=MinGW Makefiles
set BUILD_TOOL=mingw32-make

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :done_parsing
if /i "%~1"=="clean" set CLEAN_BUILD=true
if /i "%~1"=="debug" set BUILD_TYPE=Debug
if /i "%~1"=="release" set BUILD_TYPE=Release
shift
goto :parse_args
:done_parsing

echo [INFO] Building for Windows
echo [INFO] Build type: %BUILD_TYPE%

REM Get project root directory
set PROJECT_ROOT=%~dp0
set BUILD_DIR=%PROJECT_ROOT%build

REM Clean build if requested
if "%CLEAN_BUILD%"=="true" (
    echo [INFO] Cleaning build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Check for OpenCV
echo [INFO] Checking for OpenCV...
if exist "d:\00-infinuo\install" (
    echo [SUCCESS] Found OpenCV installation at d:\00-infinuo\install
) else (
    echo [WARNING] Default OpenCV path not found. Make sure OpenCV is installed and set OpenCV_DIR
)

REM Check for MinGW
where mingw32-make >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] MinGW not found in PATH. Please install MinGW-w64 or MSYS2
    echo   Download from: https://www.msys2.org/
    pause
    exit /b 1
)

REM Configure with CMake
echo [INFO] Configuring with CMake...
cmake -G "%CMAKE_GENERATOR%" ^
      -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
      "%PROJECT_ROOT%"

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

REM Build
echo [INFO] Building project...
%BUILD_TOOL% -j%NUMBER_OF_PROCESSORS%

if %errorlevel% equ 0 (
    echo [SUCCESS] Build completed successfully!
    echo [INFO] Executable location: %BUILD_DIR%\bin\ImageProcessingApp.exe
    echo [INFO] You can now run the application from the bin directory
) else (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

pause
