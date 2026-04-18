cmake_minimum_required(VERSION 3.29)
project(Precomputer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ktx CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_subdirectory("external/glad")

add_executable(Precomputer
        main.cpp
        KtxWriter.cpp
        KtxWriter.hpp
        EnvironmentBaker.cpp
        EnvironmentBaker.hpp
        BrdfLutBaker.cpp
        BrdfLutBaker.hpp
        PreviewPass.cpp
        PreviewPass.hpp
        gl/GlContext.cpp
        gl/GlContext.hpp
        gl/Shader.hpp
        gl/Shader.cpp
        file_io.hpp
        shader/brdf_lut.vert
        shader/brdf_lut.frag
        shader/preview.vert
        shader/preview.frag)

target_link_libraries(Precomputer PRIVATE
        KTX::ktx
        glfw
        glad
        glm::glm
)
