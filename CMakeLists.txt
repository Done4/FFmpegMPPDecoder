cmake_minimum_required (VERSION 2.8)
# 项目信息
project (FFmpegMPPDecoder)
# 添加头文件路径
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(/usr/local/include/opencv4)
# 添加要编译的cpp文件
file(GLOB DIR_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
# 添加链接库
set (EXTRA_LIBS ${EXTRA_LIBS}  opencv_highgui opencv_imgproc opencv_imgcodecs opencv_core avformat avdevice avfilter avcodec avutil swresample swscale rockchip_mpp)

add_executable(FFmpegMPPDecoder ${DIR_SRCS})
target_link_libraries (FFmpegMPPDecoder  ${EXTRA_LIBS})