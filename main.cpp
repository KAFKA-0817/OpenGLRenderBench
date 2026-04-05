#include <iostream>

#include "src/renderer/core/FrameBuffer.hpp"
#include "src/renderer/asset/Model.hpp"
#include "src/core/opengl.hpp"
#include "src/core/path.hpp"
#include "src/core/ThreadPool.hpp"
#include "src/editor/scene/RenderSystem.hpp"
#include "src/editor/scene/Scene.hpp"
#include "src/renderer/asset/AssetManager.hpp"
#include "src/renderer/asset/PrimitiveFactory.hpp"
#include "src/renderer/asset/importer/GltfImporter.hpp"
#include "src/renderer/asset/importer/RuntimeAssetBuilder.hpp"
#include "src/renderer/pipeline/Renderer.hpp"

#include "src/renderer/core/Shader.hpp"
#include "src/renderer/core/Window.hpp"
#include "src/renderer/camera/Camera.hpp"
#include "src/renderer/camera/OrbitController.hpp"
#include "src/renderer/camera/PerspectiveCamera.hpp"
#include "src/renderer/material/BlinnPhongMaterial.hpp"
#include "src/renderer/material/PbrMaterial.hpp"
#include "src/renderer/material/UnlitMaterial.hpp"

using namespace renderer;

int main()
{
    core::OpenGLContext context;
    constexpr int width = 1920;
    constexpr int height = 1080;
    Window window(width,height,"PBR Renderer");
    window.make_context_current();
    core::OpenGLContext::loadGlad();
    core::OpenGLContext::enableDebugOutput();
    glViewport(0,0,window.width(),window.height());

    AssetManager asset_manager;
    const auto gltf_path = core::ProjectPaths::model("ABeautifulGame\\ABeautifulGame.gltf");
    // const auto gltf_path = core::ProjectPaths::model("Lantern.glb");
    // const auto gltf_path = core::ProjectPaths::model("BoomBox.glb");
    // const auto gltf_path = core::ProjectPaths::model("2CylinderEngine.glb");
    // Model* model = asset_manager.loadModel(gltf_path);
    // std::cout << "[Import] glTF loaded: " << gltf_path << '\n';
    // std::cout << "[Import] submeshes: " << model.meshes().size() << '\n';
    // std::cout << "[Import] materials: " << model.materials().size() << '\n';
    asset_manager.requestModel(gltf_path);
    editor::Scene scene;
    editor::Entity aBeautifulGame = scene.createEntity();
    scene.addName(aBeautifulGame,{"aBeautifulGame"});

    Mesh debug_cube_mesh = PrimitiveFactory::createCube();
    Shader blinn_phong_shader(
        core::ProjectPaths::shader("blinn_phong.vs"),
        core::ProjectPaths::shader("blinn_phong.fs")
    );
    std::unique_ptr blinn_phong_material = std::make_unique<BlinnPhongMaterial>(blinn_phong_shader);
    blinn_phong_material->setAlbedo(glm::vec3(0.2f, 0.4f, 1.0f));
    Model debug_cube_model;
    debug_cube_model.add_material(std::move(blinn_phong_material));
    debug_cube_model.add_mesh(std::move(debug_cube_mesh),0);
    editor::Entity debug_cube = scene.createEntity();
    scene.addName(debug_cube,{"debug_cube"});

    Renderer renderer(width,height);
    PerspectiveCamera camera;
    OrbitController controller(window.native_handle(),&camera);

    bool reload_key_pressed_last_frame = false;
    bool key_1_last_frame = false;
    bool key_2_last_frame = false;
    bool key_3_last_frame = false;
    bool key_4_last_frame = false;
    bool key_5_last_frame = false;
    bool key_6_last_frame = false;
    while (!window.should_close())
    {
        asset_manager.pumpUploads();
        window.poll_events();
        controller.update();
        const int current_width = window.width();
        const int current_height = window.height();

        if (current_width > 0 && current_height > 0 &&
            (current_width != renderer.width() || current_height != renderer.height()))
        {
            renderer.resize(current_width, current_height);
            camera.setAspect(static_cast<float>(current_width) / static_cast<float>(current_height));
            glViewport(0, 0, current_width, current_height);
        }

        bool reload_key_pressed = glfwGetKey(window.native_handle(), GLFW_KEY_R) == GLFW_PRESS;
        if (reload_key_pressed && !reload_key_pressed_last_frame) {
            renderer.reloadBuiltinShaders();
            blinn_phong_shader.reload();
        }
        reload_key_pressed_last_frame = reload_key_pressed;

        bool key_1 = glfwGetKey(window.native_handle(), GLFW_KEY_1) == GLFW_PRESS;
        if (key_1 && !key_1_last_frame) renderer.setPreviewMode(PreviewMode::FinalScene);
        key_1_last_frame = key_1;

        bool key_2 = glfwGetKey(window.native_handle(), GLFW_KEY_2) == GLFW_PRESS;
        if (key_2 && !key_2_last_frame) renderer.setPreviewMode(PreviewMode::GPosition);
        key_2_last_frame = key_2;

        bool key_3 = glfwGetKey(window.native_handle(), GLFW_KEY_3) == GLFW_PRESS;
        if (key_3 && !key_3_last_frame) renderer.setPreviewMode(PreviewMode::GNormal);
        key_3_last_frame = key_3;

        bool key_4 = glfwGetKey(window.native_handle(), GLFW_KEY_4) == GLFW_PRESS;
        if (key_4 && !key_4_last_frame) renderer.setPreviewMode(PreviewMode::GAlbedo);
        key_4_last_frame = key_4;

        bool key_5 = glfwGetKey(window.native_handle(), GLFW_KEY_5) == GLFW_PRESS;
        if (key_5 && !key_5_last_frame) renderer.setPreviewMode(PreviewMode::GMaterial);
        key_5_last_frame = key_5;

        bool key_6 = glfwGetKey(window.native_handle(), GLFW_KEY_6) == GLFW_PRESS;
        if (key_6 && !key_6_last_frame) renderer.setPreviewMode(PreviewMode::GEmissive);
        key_6_last_frame = key_6;

        renderer.clearSubmissions();

        float time = static_cast<float>(glfwGetTime());
        float angle = glm::radians(45.0f) * time;
        glm::mat4 world = glm::mat4(1.0f);
        world = glm::translate(world, glm::vec3(0.0f, 0.0f, 0.0f));
        // world = glm::rotate(world, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        // world = glm::rotate(world, angle, glm::vec3(1.0f, 0.0f, 0.0f));
        // world = glm::rotate(world, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        world = glm::scale(world, glm::vec3(3.0f));

        scene.addTransform(aBeautifulGame,{
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f)
        });

        Model* model = asset_manager.tryGetModel(gltf_path);
        if (model !=nullptr) {
            scene.addMeshRenderer(aBeautifulGame,{model,true});
        }


        glm::mat4 debug_transform = glm::mat4(1.0f);
        debug_transform = glm::translate(debug_transform, glm::vec3(2.0f, 0.0f, 0.0f));
        debug_transform = glm::rotate(debug_transform, glm::radians(35.0f),
                                      glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
        debug_transform = glm::scale(debug_transform, glm::vec3(0.5f));
        scene.addTransform(debug_cube,{
            glm::vec3(2.0f, 0.0f, 0.0f),
            glm::vec3(35.0f, 35.0f, 0.0f),
            glm::vec3(0.5f)
        });
        scene.addMeshRenderer(debug_cube,{&debug_cube_model,true});
        // renderer.submit(debug_cube_model,blinn_phong_material,debug_transform);

        editor::RenderSystem::renderScene(renderer,scene);
        renderer.renderFrame(camera);

        window.swap_buffers();
    }

    return 0;
}
