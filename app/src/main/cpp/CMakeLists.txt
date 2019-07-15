#指定支持的最低版本
cmake_minimum_required(VERSION 3.4.1)


file(GLOB sourceFile
        ./*.cpp ./*/*.cpp ./*/*/*.cpp
        ./*.h ./*/*.h ./*/*/*.h
        ./*.c ./*/*.c ./*/*/*.c
        )

include_directories(ffmpeg/include/ ./ player)


add_library(myffmpeg SHARED ${sourceFile})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/ffmpeg/libs/${ANDROID_ABI}")

message(WARNING ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI})


#find_library(log log)
# 链接其他库，从上面设置的目录中找
target_link_libraries(myffmpeg
        log
        avformat
        avcodec
        avfilter
        avutil
        swresample
        swscale
        z
        android
        )