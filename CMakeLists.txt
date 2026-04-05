cmake_minimum_required(VERSION 4.1)
project(PBRRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_subdirectory("external/glad")
add_subdirectory("external/stb_image")
add_subdirectory("external/tinygltf")

add_executable(PBRRenderer main.cpp
        src/core/file_io.hpp
        src/core/noncopyable.hpp
        src/renderer/core/Shader.cpp
        src/renderer/core/Shader.hpp
        src/renderer/core/Window.cpp
        src/renderer/core/Window.hpp
        src/renderer/asset/Mesh.cpp
        src/renderer/asset/Mesh.hpp
        src/core/opengl.hpp
        src/renderer/asset/Model.cpp
        src/renderer/asset/Model.hpp
        src/renderer/asset/PrimitiveFactory.cpp
        src/renderer/asset/PrimitiveFactory.hpp
        src/renderer/camera/Camera.hpp
        src/renderer/camera/PerspectiveCamera.cpp
        src/renderer/camera/PerspectiveCamera.hpp
        src/renderer/camera/OrthographicCamera.cpp
        src/renderer/camera/OrthographicCamera.hpp
        src/renderer/pipeline/Renderer.cpp
        src/renderer/pipeline/Renderer.hpp
        src/renderer/core/FrameBuffer.cpp
        src/renderer/core/FrameBuffer.hpp
        src/core/opengl.cpp
        src/core/path.hpp
        src/core/path.cpp
        src/renderer/material/Material.hpp
        src/renderer/material/UnlitMaterial.cpp
        src/renderer/material/UnlitMaterial.hpp
        src/renderer/material/BlinnPhongMaterial.cpp
        src/renderer/material/BlinnPhongMaterial.hpp
        src/renderer/material/PbrMaterial.cpp
        src/renderer/material/PbrMaterial.hpp
        src/renderer/asset/Texture2D.cpp
        src/renderer/asset/Texture2D.hpp
        src/renderer/pipeline/RenderPass.hpp
        src/renderer/pipeline/PresentPass.cpp
        src/renderer/pipeline/PresentPass.hpp
        src/renderer/pipeline/NormalPass.cpp
        src/renderer/pipeline/NormalPass.hpp
        src/renderer/pipeline/FragPosPass.cpp
        src/renderer/pipeline/FragPosPass.hpp
        src/renderer/pipeline/RenderItem.hpp
        src/renderer/pipeline/GBufferPass.cpp
        src/renderer/pipeline/GBufferPass.hpp
        src/renderer/pipeline/LightingPass.cpp
        src/renderer/pipeline/LightingPass.hpp
        src/renderer/pipeline/ForwardPass.cpp
        src/renderer/pipeline/ForwardPass.hpp
        src/renderer/material/ForwardMaterial.hpp
        src/renderer/material/DeferredMaterial.hpp
        src/renderer/asset/importer/ImportedModelData.hpp
        src/renderer/asset/Vertex.hpp
        src/renderer/asset/importer/GltfImporter.cpp
        src/renderer/asset/importer/GltfImporter.hpp
        src/renderer/asset/importer/RuntimeAssetBuilder.cpp
        src/renderer/asset/importer/RuntimeAssetBuilder.hpp
        src/core/glfw_globals.hpp
        src/renderer/camera/OrbitController.cpp
        src/renderer/camera/OrbitController.hpp
        src/renderer/asset/AssetManager.cpp
        src/renderer/asset/AssetManager.hpp
        src/core/ThreadPool.cpp
        src/core/ThreadPool.hpp
        src/editor/scene/Entity.hpp
        src/editor/scene/Components.hpp
        src/editor/scene/Scene.cpp
        src/editor/scene/Scene.hpp
        src/editor/scene/RenderSystem.hpp
        src/editor/scene/RenderSystem.cpp
)

target_link_libraries(PBRRenderer PRIVATE
        glfw
        glad
        glm::glm
        assimp::assimp
        stb_image
        tinygltf
)