cmake_minimum_required(VERSION 3.10)
project(ImageProcessingApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set OpenCV path
set(OpenCV_DIR "d:/00-infinuo/install/x64/mingw/lib")

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/third_party/cvui)

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

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Windows specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} comdlg32)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy test images to build directory
file(COPY ${CMAKE_SOURCE_DIR}/images DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Copy OpenCV DLLs to build directory (Windows only)
if(WIN32)
    file(GLOB OPENCV_DLLS "d:/00-infinuo/install/x64/mingw/bin/*.dll")
    file(COPY ${OPENCV_DLLS} DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()

# Print OpenCV information
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
