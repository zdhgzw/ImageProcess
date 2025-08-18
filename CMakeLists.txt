cmake_minimum_required(VERSION 3.10)
project(ImageProcessingApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable all compiler warnings globally
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

# Suppress specific warnings for different compilers
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0")
endif()

# Platform detection
if(WIN32)
    message(STATUS "Building for Windows")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
elseif(APPLE)
    message(STATUS "Building for macOS")
endif()

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Use custom OpenCV finder if OpenCV_DIR is not set
if(NOT DEFINED OpenCV_DIR)
    include(FindOpenCVCustom)
endif()

# Find OpenCV
find_package(OpenCV REQUIRED)

# Suppress OpenCV related warnings
add_definitions(-DOPENCV_DISABLE_EIGEN_TENSOR_SUPPORT)
add_definitions(-D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
add_definitions(-DOPENCV_DISABLE_THREAD_SUPPORT)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/third_party/cvui)

# Compiler specific settings
if(MSVC)
    # Visual Studio specific settings - disable warnings
    add_compile_options(/W0)  # Disable all warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC/Clang specific settings - disable warnings
    add_compile_options(-w)  # Disable all warnings
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/ImageProcessingApp.cpp
    src/ImageProcessor.cpp
    src/PreProcessing.cpp
    src/Segmentation.cpp
    src/Morphology.cpp
    src/CleanUp.cpp
    src/Measurements.cpp
    src/UIComponents.cpp
)

# Headers
set(HEADERS
    include/ImageProcessingApp.h
    include/ImageProcessor.h
    include/PreProcessing.h
    include/Segmentation.h
    include/Morphology.h
    include/CleanUp.h
    include/Measurements.h
    include/UIComponents.h
    third_party/cvui/cvui.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Target include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Disable warnings for this target specifically
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -w)
endif()

# Set additional properties to suppress warnings
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_WARNING_AS_ERROR OFF
)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Platform specific libraries
if(WIN32)
    # Windows specific libraries
    target_link_libraries(${PROJECT_NAME} comdlg32)
elseif(UNIX AND NOT APPLE)
    # Linux specific libraries
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)

    # X11 libraries for GUI (if available)
    find_package(X11)
    if(X11_FOUND)
        target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
        message(STATUS "X11 libraries found and linked")
    endif()

    # GTK libraries for file dialogs (optional)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GTK3 QUIET gtk+-3.0)
        if(GTK3_FOUND)
            target_include_directories(${PROJECT_NAME} PRIVATE ${GTK3_INCLUDE_DIRS})
            target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})
            target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_GTK3)
            message(STATUS "GTK3 found and linked for file dialogs")
        endif()
    endif()
elseif(APPLE)
    # macOS specific libraries
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy test images to build directory (if exists)
if(EXISTS ${CMAKE_SOURCE_DIR}/images)
    file(COPY ${CMAKE_SOURCE_DIR}/images DESTINATION ${CMAKE_BINARY_DIR}/bin)
    message(STATUS "Test images copied to build directory")
endif()

# Platform specific post-build steps
if(WIN32)
    # Copy OpenCV DLLs to build directory (Windows only)
    if(EXISTS "d:/00-infinuo/install/x64/mingw/bin")
        file(GLOB OPENCV_DLLS "d:/00-infinuo/install/x64/mingw/bin/*.dll")
        if(OPENCV_DLLS)
            file(COPY ${OPENCV_DLLS} DESTINATION ${CMAKE_BINARY_DIR}/bin)
            message(STATUS "OpenCV DLLs copied to build directory")
        endif()
    endif()

    # Set Windows subsystem for GUI application
    if(MSVC)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux specific settings
    # Set RPATH for shared libraries
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )

    # Create desktop entry (optional)
    configure_file(
        ${CMAKE_SOURCE_DIR}/scripts/ImageProcessingApp.desktop.in
        ${CMAKE_BINARY_DIR}/ImageProcessingApp.desktop
        @ONLY
    )
endif()

# Installation rules (Linux/Unix)
if(UNIX AND NOT APPLE)
    # Install executable
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )

    # Install desktop entry
    if(EXISTS ${CMAKE_BINARY_DIR}/ImageProcessingApp.desktop)
        install(FILES ${CMAKE_BINARY_DIR}/ImageProcessingApp.desktop
            DESTINATION share/applications
        )
    endif()

    # Install documentation
    install(FILES ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_SOURCE_DIR}/BUILD.md
        DESTINATION share/doc/${PROJECT_NAME}
        OPTIONAL
    )
endif()

# Print build information
message(STATUS "=== Build Configuration ===")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "=== OpenCV Information ===")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV include path: ${OpenCV_INCLUDE_DIRS}")
if(WIN32)
    message(STATUS "=== Windows Specific ===")
    message(STATUS "Output directory: ${CMAKE_BINARY_DIR}/bin")
endif()
if(UNIX)
    message(STATUS "=== Unix Specific ===")
    message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
endif()
