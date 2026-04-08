#include <iostream>

#include "src/renderer/core/FrameBuffer.hpp"
#include "src/renderer/asset/Model.hpp"
#include "src/core/opengl.hpp"
#include "src/core/path.hpp"
#include "src/core/ThreadPool.hpp"
#include "src/editor/scene/RenderSystem.hpp"
#include "src/editor/scene/LightSystem.hpp"
#include "src/editor/scene/Scene.hpp"
#include "src/renderer/asset/AssetManager.hpp"
#include "src/renderer/asset/PrimitiveFactory.hpp"
#include "src/renderer/asset/importer/GltfImporter.hpp"
#include "src/renderer/asset/importer/RuntimeAssetBuilder.hpp"
#include "src/renderer/pipeline/RenderContextFrame.hpp"
#include "src/renderer/pipeline/Renderer.hpp"

#include "src/renderer/core/Shader.hpp"
#include "src/renderer/core/Window.hpp"
#include "src/renderer/camera/Camera.hpp"
#include "src/renderer/camera/OrbitController.hpp"
#include "src/renderer/camera/PerspectiveCamera.hpp"
#include "src/renderer/material/BlinnPhongMaterial.hpp"
#include "src/renderer/material/PbrMaterial.hpp"
#include "src/renderer/material/UnlitMaterial.hpp"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "src/editor/EditorUi.hpp"

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // 开启 Docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // 你如果以后想多 viewport，再开这个
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.native_handle(), true);
    ImGui_ImplOpenGL3_Init("#version 410");

    AssetManager asset_manager;
    // const auto gltf_path = core::ProjectPaths::model("ABeautifulGame\\ABeautifulGame.gltf");
    // const auto gltf_path = core::ProjectPaths::model("Lantern.glb");
    // const auto gltf_path = core::ProjectPaths::model("BoomBox.glb");
    // const auto gltf_path = core::ProjectPaths::model("2CylinderEngine.glb");
    asset_manager.requestModel(core::ProjectPaths::model("ABeautifulGame\\ABeautifulGame.gltf"));
    asset_manager.requestModel(core::ProjectPaths::model("Lantern.glb"));
    asset_manager.requestModel(core::ProjectPaths::model("BoomBox.glb"));

    editor::Scene scene;
    editor::Entity main_light = scene.createEntity();
    scene.addName(main_light, {"Main Light"});
    scene.addTransform(main_light, {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-70.0f, 33.0f, 0.0f),
        glm::vec3(1.0f)
    });
    scene.addDirectionalLight(main_light, {});

    editor::Entity aBeautifulGame = scene.createEntity();
    scene.addName(aBeautifulGame,{"aBeautifulGame"});
    scene.addTransform(aBeautifulGame,{
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f)
        });

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
    scene.addTransform(debug_cube,{
        glm::vec3(2.0f, 0.0f, 0.0f),
        glm::vec3(35.0f, 35.0f, 0.0f),
        glm::vec3(0.5f)
    });
    scene.addMeshRenderer(debug_cube,{&debug_cube_model,true});

    Renderer renderer(width,height);
    RenderContextFrame render_context_frame;
    renderer.setPresentToScreenEnabled(false);
    PerspectiveCamera camera;
    OrbitController controller(window.native_handle(),&camera);

    editor::EditorUI editor_ui(scene, renderer,asset_manager);

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const auto viewport_size = editor_ui.state().viewport_size;
        if (viewport_size.x > 0 && viewport_size.y > 0 &&
            (viewport_size.x != renderer.width() || viewport_size.y != renderer.height()))
        {
            renderer.resize(viewport_size.x, viewport_size.y);
            camera.setAspect(static_cast<float>(viewport_size.x) / static_cast<float>(viewport_size.y));
        }

        if (editor_ui.state().viewport_hovered || editor_ui.state().viewport_focused) {
            controller.update();
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
        render_context_frame.beginFrame();
        render_context_frame.writable().camera_position = camera.position();

        editor::LightSystem::writeLights(render_context_frame, scene);
        editor::RenderSystem::renderScene(renderer, scene);
        renderer.renderFrame(camera, render_context_frame.context());

        editor_ui.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swap_buffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
