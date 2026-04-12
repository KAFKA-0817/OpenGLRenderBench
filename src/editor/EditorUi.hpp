//
// Created by kaede on 2026/4/7.
//

#ifndef PBRRENDERER_EDITORUI_HPP
#define PBRRENDERER_EDITORUI_HPP
#include "EditorState.hpp"
#include "../app/frame/EditorCommandsFrame.hpp"
#include "../core/Log.hpp"
#include "../core/noncopyable.hpp"
#include "../renderer/asset/AssetManager.hpp"
#include "../renderer/camera/Camera.hpp"
#include "../renderer/pipeline/Renderer.hpp"
#include "scene/Scene.hpp"

namespace editor {

class EditorUI : public core::NonCopyable{
public:
    EditorUI(Scene& scene,
             renderer::Renderer& renderer,
             const renderer::Camera& camera,
             renderer::AssetManager& assetManager,
             app::EditorCommandsFrame& commands_frame)
        :scene_(scene),renderer_(renderer),camera_(camera),assetManager_(assetManager),commands_frame_(commands_frame){}
    ~EditorUI() = default;
    EditorUI(EditorUI&& other) noexcept = delete;
    EditorUI& operator=(EditorUI&& other) noexcept = delete;

    void draw();

    EditorState& state() noexcept { return state_; }
    const EditorState& state() const noexcept { return state_; }

private:
    void drawDockSpace();
    void drawRendererPanel();
    void drawHierarchy();
    void drawInspector();
    void drawViewport();
    void drawConsole();

private:
    EditorState state_;
    Scene& scene_;
    renderer::Renderer& renderer_;
    const renderer::Camera& camera_;
    const renderer::AssetManager& assetManager_;
    app::EditorCommandsFrame& commands_frame_;

    bool dockspace_initialized_ = false;

    Entity rename_target_ = kInvalidEntity;
    std::string rename_buffer_;
};

    enum class ComponentRemoveRequest {
        Name,
        Transform,
        MeshRenderer,
        DirectionalLight,
        PointLight
    };

} // editor

#endif //PBRRENDERER_EDITORUI_HPP
