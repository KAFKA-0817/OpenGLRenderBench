//
// Created by kaede on 2026/4/21.
//

#include "Application.hpp"

#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../core/Log.hpp"
#include "../core/path.hpp"
#include "../renderer/asset/PrimitiveFactory.hpp"
#include "../renderer/material/BlinnPhongMaterial.hpp"
#include "../core/FileDialog.hpp"
#include "../editor/scene/LightSystem.hpp"
#include "../editor/scene/RenderSystem.hpp"

namespace app {
    Application::Application(const ApplicationConfig& config) 
    :app_window_(config.width,config.height,config.title)
    ,shader_directory_watcher_(ProjectPaths::shaders())
    {
        initOpenGL();
        initImGui();
        renderer_.emplace(900,600);
        controller_.emplace(app_window_.native_handle(),&camera_);
        initRenderer();
        createDefaultScene();
        ui_.emplace(scene_, *renderer_, camera_, asset_manager_, editor_commands_frame_);
    }

    Application::~Application() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::initOpenGL() const {
        app_window_.make_context_current();
        opengl_context_.loadGlad();
        opengl_context_.loadKtxOpenGL();
        opengl_context_.enableDebugOutput();
        glViewport(0,0,app_window_.width(),app_window_.height());
    }

    void Application::initImGui() const {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // 开启 Docking
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();
        constexpr float kUiScale = 1.5f;
        ImGui::GetStyle().ScaleAllSizes(kUiScale);
        io.FontGlobalScale = kUiScale;

        ImGui_ImplGlfw_InitForOpenGL(app_window_.native_handle(), true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void Application::initRenderer() {
        renderer_->loadBrdfLut(ProjectPaths::assets() / "brdf_lut.ktx2");
        const auto sunny_sky_dir = ProjectPaths::assets() / "ibl" / "SunnySky";
        renderer_->loadEnvironmentMap(sunny_sky_dir / "SunnySky_env.ktx2");
        renderer_->loadIrradianceMap(sunny_sky_dir / "SunnySky_irradiance.ktx2");
        renderer_->loadPrefilterMap(sunny_sky_dir / "SunnySky_prefilter.ktx2");
    }

    void Application::createDefaultScene() {
        asset_manager_.requestModel(ProjectPaths::model("ABeautifulGame\\ABeautifulGame.gltf"));
        asset_manager_.requestModel(ProjectPaths::model("Lantern.glb"));
        asset_manager_.requestModel(ProjectPaths::model("BoomBox.glb"));
        asset_manager_.requestModel(ProjectPaths::model("TwoSidedPlane\\TwoSidedPlane.gltf"));
        asset_manager_.requestModel(ProjectPaths::model("MarbleCube\\Cube.gltf"));
        asset_manager_.requestModel(ProjectPaths::model("MetalRoughSpheres.glb"));

        //Main Light
        auto mainLight = scene_.createEntity();
        scene_.addName(mainLight, {"Main Light"});
        scene_.addTransform(mainLight, TransformComponent::fromEulerDegrees(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(-70.0f, 33.0f, 0.0f)
        ));
        scene_.addDirectionalLight(mainLight, {});

        //Debug-cube
        Mesh debug_cube_mesh = PrimitiveFactory::createCube();
        debug_blinn_phong_shader_.emplace(
            ProjectPaths::shader("blinn_phong.vs"),
            ProjectPaths::shader("blinn_phong.fs")
        );
        std::unique_ptr blinn_phong_material = std::make_unique<BlinnPhongMaterial>(*debug_blinn_phong_shader_);
        blinn_phong_material->setAlbedo(glm::vec3(0.2f, 0.4f, 1.0f));
        debug_cube_model_.emplace();
        debug_cube_model_->add_material(std::move(blinn_phong_material));
        debug_cube_model_->add_mesh(std::move(debug_cube_mesh),0);
        Entity debug_cube = scene_.createEntity();
        scene_.addName(debug_cube,{"debug_cube"});
        scene_.addTransform(debug_cube, editor::TransformComponent::fromEulerDegrees(
            glm::vec3(2.0f, 0.0f, 0.0f),
            glm::vec3(35.0f, 35.0f, 0.0f),
            glm::vec3(0.5f)
        ));
        scene_.addMeshRenderer(debug_cube,{&*debug_cube_model_,true});
    }

    void Application::parseCommandFrame() {
        //resize window
        const auto viewport_size = ui_->state().viewport_size;
        if (viewport_size.x > 0 && viewport_size.y > 0 && (viewport_size.x != renderer_->width() || viewport_size.y != renderer_->height()))
        {
            renderer_->resize(viewport_size.x, viewport_size.y);
            camera_.setAspect(static_cast<float>(viewport_size.x) / static_cast<float>(viewport_size.y));
        }

        //update controller
        if (ui_->state().viewport_content_hovered && !ui_->state().gizmo_over && !ui_->state().gizmo_using) {
            controller_->update(ImGui::GetIO().MouseWheel);
        }

        //draw ui
        editor_commands_frame_.beginFrame();
        render_context_frame_.beginFrame();
        ui_->draw();

        // open file dialog
        if (editor_commands_frame_.commands().open_model) {
            if (auto path = FileDialog::openGltfModel()) {
                asset_manager_.requestModel(*path);
                Log::getInstance().write("Asset", "Requested model: " + path->string());
            }
        }

        // reload shader
        if (editor_commands_frame_.commands().reload_shaders) {
            renderer_->reloadBuiltinShaders();
            if (debug_blinn_phong_shader_) {
                debug_blinn_phong_shader_->reload();
            }
        }
        renderer_->reloadShaders(shader_directory_watcher_.consumeDirtyFiles());

        // select item
        if (editor_commands_frame_.commands().frame_selected) {
            const Entity selected_entity = ui_->state().selectedEntity;
            if (selected_entity != kInvalidEntity) {
                if (const auto* transform = scene_.tryGetTransform(selected_entity)) {
                    controller_->focusTarget(transform->position);
                }
            }
        }
    }

    void Application::parseRenderFrame() {
        renderer_->clearSubmissions();
        render_context_frame_.writable().camera_position = camera_.position();
        render_context_frame_.writable().selected = ui_->state().selectedEntity;
        LightSystem::writeLights(render_context_frame_, scene_);
        RenderSystem::renderScene(*renderer_, scene_);
        renderer_->renderFrame(camera_, render_context_frame_.context());
    }

    void Application::run() {
        shader_directory_watcher_.start();

        double fps_window_start_time = glfwGetTime();
        int fps_window_frame_count = 0;
        while (!app_window_.should_close()) {
            app_window_.poll_events();
            asset_manager_.pumpUploads();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //fps count
            const double current_time = glfwGetTime();
            ++fps_window_frame_count;
            const double fps_window_elapsed = current_time - fps_window_start_time;
            if (fps_window_elapsed >= 0.25) {
                ui_->state().display_fps = static_cast<float>(fps_window_frame_count / fps_window_elapsed);
                fps_window_start_time = current_time;
                fps_window_frame_count = 0;
            }

            parseCommandFrame();
            parseRenderFrame();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            app_window_.swap_buffers();
        }
    }
} // app
