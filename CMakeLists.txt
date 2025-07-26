cmake_minimum_required(VERSION 3.16)
project(ImageProcessingWithOpenCV)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译器选项
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

# 尝试使用静态OpenCV库
set(OpenCV_STATIC ON)

# 设置可能的OpenCV安装路径
set(OPENCV_POSSIBLE_PATHS
    "D:/00-infinuo/install"
    "/d/00-infinuo/install"
    "D:\\00-infinuo\\install"
    "C:/opencv"
    "C:/opencv/build"
    "$ENV{OPENCV_DIR}"
)

# 查找OpenCV安装目录
set(OPENCV_FOUND_PATH "")
foreach(OPENCV_PATH ${OPENCV_POSSIBLE_PATHS})
    if(EXISTS "${OPENCV_PATH}")
        set(OPENCV_FOUND_PATH "${OPENCV_PATH}")
        message(STATUS "Found OpenCV installation at: ${OPENCV_FOUND_PATH}")
        break()
    endif()
endforeach()

# 如果找到OpenCV安装目录，设置相关路径
if(OPENCV_FOUND_PATH)
    # 尝试不同的CMake配置文件位置
    set(OPENCV_CMAKE_PATHS
        "${OPENCV_FOUND_PATH}/lib/cmake/opencv4"
        "${OPENCV_FOUND_PATH}/cmake"
        "${OPENCV_FOUND_PATH}/lib/cmake"
        "${OPENCV_FOUND_PATH}/share/opencv4"
        "${OPENCV_FOUND_PATH}/lib/cmake/opencv"
        "${OPENCV_FOUND_PATH}"
    )

    foreach(CMAKE_PATH ${OPENCV_CMAKE_PATHS})
        if(EXISTS "${CMAKE_PATH}")
            set(OpenCV_DIR "${CMAKE_PATH}")
            message(STATUS "Setting OpenCV_DIR to: ${OpenCV_DIR}")
            break()
        endif()
    endforeach()

    # 添加到CMAKE_PREFIX_PATH
    list(APPEND CMAKE_PREFIX_PATH "${OPENCV_FOUND_PATH}")
else()
    message(WARNING "OpenCV installation not found in expected locations. Please set OpenCV_DIR manually.")
endif()

# 查找OpenCV
find_package(OpenCV REQUIRED)

# 包含目录
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

# 输出OpenCV信息
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# 创建通用工具库
add_library(ImageProcessingUtils STATIC
    src/utils/ImageUtils.cpp
    src/utils/ArgumentParser.cpp
)

target_link_libraries(ImageProcessingUtils ${OpenCV_LIBS})

# 图像处理算法可执行文件
set(ALGORITHMS
    grayscale_converter
    edge_detector
    image_blur
    particle_analysis
    particle_segment
)

# 为每个算法创建可执行文件
foreach(ALGORITHM ${ALGORITHMS})
    add_executable(${ALGORITHM} src/algorithms/${ALGORITHM}.cpp)
    target_link_libraries(${ALGORITHM} ImageProcessingUtils ${OpenCV_LIBS})
    
    # 设置输出目录
    set_target_properties(${ALGORITHM} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# 安装规则
install(TARGETS ${ALGORITHMS} DESTINATION bin)
install(DIRECTORY examples/ DESTINATION examples)

# 打印构建信息
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")
