//
// Created by kaede on 2026/4/21.
//

#ifndef OPENGLRENDERBENCH_APPLICATION_HPP
#define OPENGLRENDERBENCH_APPLICATION_HPP
#include <optional>

#include "../core/DirectoryWatcher.hpp"
#include "../core/noncopyable.hpp"
#include "../core/opengl.hpp"
#include "../editor/scene/Scene.hpp"
#include "../renderer/asset/AssetManager.hpp"
#include "../renderer/asset/Model.hpp"
#include "../renderer/core/Window.hpp"
#include "../renderer/core/Shader.hpp"
#include "../renderer/pipeline/Renderer.hpp"
#include "frame/EditorCommandsFrame.hpp"
#include "frame/RenderContextFrame.hpp"
#include "../renderer/camera/OrbitController.hpp"
#include "../renderer/camera/PerspectiveCamera.hpp"
#include "../editor/EditorUi.hpp"

namespace app {
    using namespace core;
    using namespace renderer;
    using namespace editor;

    struct ApplicationConfig {
        int width = 1280;
        int height = 720;
        std::string title = "OpenGLRenderBench";
    };

    class Application : public NonCopyable {
    public:
        Application(const ApplicationConfig& config);
        ~Application();
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void run();

    private:
        void initOpenGL() const;
        void initImGui() const;
        void initRenderer();
        void createDefaultScene();
        void parseCommandFrame();
        void parseRenderFrame();

    private:
        OpenGLContext opengl_context_;
        Window app_window_;
        AssetManager asset_manager_;
        DirectoryWatcher shader_directory_watcher_;
        EditorCommandsFrame editor_commands_frame_;
        RenderContextFrame render_context_frame_;
        PerspectiveCamera camera_;
        std::optional<Shader> debug_blinn_phong_shader_;
        std::optional<Model> debug_cube_model_;
        Scene scene_;
        std::optional<Renderer> renderer_;
        std::optional<OrbitController> controller_;
        std::optional<EditorUI> ui_;
    };

} // app

#endif //OPENGLRENDERBENCH_APPLICATION_HPP
