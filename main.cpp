#include <iostream>

#include "src/renderer/core/FrameBuffer.hpp"
#include "src/renderer/asset/Model.hpp"
#include "src/core/opengl.hpp"
#include "src/core/path.hpp"
#include "src/editor/scene/RenderSystem.hpp"
#include "src/editor/scene/LightSystem.hpp"
#include "src/editor/scene/Scene.hpp"
#include "src/renderer/asset/AssetManager.hpp"
#include "src/renderer/asset/PrimitiveFactory.hpp"
#include "src/app/frame/EditorCommandsFrame.hpp"
#include "src/app/frame/RenderContextFrame.hpp"
#include "src/renderer/pipeline/Renderer.hpp"

#include "src/renderer/core/Shader.hpp"
#include "src/renderer/core/Window.hpp"
#include "src/renderer/camera/Camera.hpp"
#include "src/renderer/camera/OrbitController.hpp"
#include "src/renderer/camera/PerspectiveCamera.hpp"
#include "src/renderer/material/BlinnPhongMaterial.hpp"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "src/editor/EditorUi.hpp"

using namespace renderer;

int main()
{
    core::OpenGLContext context;
    constexpr int width = 3840;
    constexpr int height = 2160;
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

    // 多viewport，再开这个
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    constexpr float kUiScale = 1.5f;
    ImGui::GetStyle().ScaleAllSizes(kUiScale);
    io.FontGlobalScale = kUiScale;

    ImGui_ImplGlfw_InitForOpenGL(window.native_handle(), true);
    ImGui_ImplOpenGL3_Init("#version 410");

    AssetManager asset_manager;
    asset_manager.requestModel(core::ProjectPaths::model("ABeautifulGame\\ABeautifulGame.gltf"));
    asset_manager.requestModel(core::ProjectPaths::model("Lantern.glb"));
    asset_manager.requestModel(core::ProjectPaths::model("BoomBox.glb"));

    editor::Scene scene;
    editor::Entity main_light = scene.createEntity();
    scene.addName(main_light, {"Main Light"});
    scene.addTransform(main_light, editor::TransformComponent::fromEulerDegrees(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-70.0f, 33.0f, 0.0f)
    ));
    scene.addDirectionalLight(main_light, {});

    editor::Entity aBeautifulGame = scene.createEntity();
    scene.addName(aBeautifulGame,{"aBeautifulGame"});
    scene.addTransform(aBeautifulGame, editor::TransformComponent::fromEulerDegrees(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(3.0f)
    ));

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
    scene.addTransform(debug_cube, editor::TransformComponent::fromEulerDegrees(
        glm::vec3(2.0f, 0.0f, 0.0f),
        glm::vec3(35.0f, 35.0f, 0.0f),
        glm::vec3(0.5f)
    ));
    scene.addMeshRenderer(debug_cube,{&debug_cube_model,true});

    Renderer renderer(900,600);
    app::EditorCommandsFrame editor_commands_frame;
    RenderContextFrame render_context_frame;
    PerspectiveCamera camera;
    OrbitController controller(window.native_handle(),&camera);
    double fps_window_start_time = glfwGetTime();
    int fps_window_frame_count = 0;

    editor::EditorUI editor_ui(scene, renderer, camera, asset_manager, editor_commands_frame);
    while (!window.should_close())
    {
        asset_manager.pumpUploads();
        window.poll_events();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const double current_time = glfwGetTime();
        ++fps_window_frame_count;
        const double fps_window_elapsed = current_time - fps_window_start_time;
        if (fps_window_elapsed >= 0.25) {
            editor_ui.state().display_fps = static_cast<float>(fps_window_frame_count / fps_window_elapsed);
            fps_window_start_time = current_time;
            fps_window_frame_count = 0;
        }

        const auto viewport_size = editor_ui.state().viewport_size;
        if (viewport_size.x > 0 && viewport_size.y > 0 &&
            (viewport_size.x != renderer.width() || viewport_size.y != renderer.height()))
        {
            renderer.resize(viewport_size.x, viewport_size.y);
            camera.setAspect(static_cast<float>(viewport_size.x) / static_cast<float>(viewport_size.y));
        }

        if (editor_ui.state().viewport_content_hovered && !editor_ui.state().gizmo_over && !editor_ui.state().gizmo_using) {
            controller.update(io.MouseWheel);
        }

        editor_commands_frame.beginFrame();
        render_context_frame.beginFrame();
        editor_ui.draw();

        if (editor_commands_frame.commands().reload_shaders) {
            renderer.reloadBuiltinShaders();
            blinn_phong_shader.reload();
        }

        if (editor_commands_frame.commands().frame_selected) {
            const editor::Entity selected_entity = editor_ui.state().selectedEntity;
            if (selected_entity != editor::kInvalidEntity) {
                if (const auto* transform = scene.tryGetTransform(selected_entity)) {
                    controller.focusTarget(transform->position);
                }
            }
        }

        renderer.clearSubmissions();
        render_context_frame.writable().camera_position = camera.position();
        render_context_frame.writable().selected = editor_ui.state().selectedEntity;

        editor::LightSystem::writeLights(render_context_frame, scene);
        editor::RenderSystem::renderScene(renderer, scene);
        renderer.renderFrame(camera, render_context_frame.context());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swap_buffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
