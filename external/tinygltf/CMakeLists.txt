cmake_minimum_required(VERSION 4.1)
project(tinygltf)

add_library(tinygltf STATIC
        tiny_gltf.cc
)

target_include_directories(tinygltf
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(tinygltf PUBLIC cxx_std_17)