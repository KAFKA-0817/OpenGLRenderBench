cmake_minimum_required(VERSION 4.1)
project(stb_image)

add_library(stb_image STATIC
        stb_image.cpp
)

target_include_directories(stb_image
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)