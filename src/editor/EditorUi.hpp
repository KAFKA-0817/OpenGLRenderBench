//
// Created by kaede on 2026/4/7.
//

#ifndef PBRRENDERER_EDITORUI_HPP
#define PBRRENDERER_EDITORUI_HPP
#include "EditorState.hpp"
#include "../core/noncopyable.hpp"
#include "scene/Scene.hpp"

namespace editor {

class EditorUI : public core::NonCopyable{
public:
    EditorUI(Scene& scene):scene_(scene){}
    ~EditorUI() = default;
    EditorUI(EditorUI&& other) noexcept = delete;
    EditorUI& operator=(EditorUI&& other) noexcept = delete;

    void draw();

    EditorState& state() noexcept { return state_; }
    const EditorState& state() const noexcept { return state_; }

private:
    void drawDockSpace();
    void drawHierarchy();
    void drawInspector();
    void drawViewport();

private:
    EditorState state_;
    Scene& scene_;
};

} // editor

#endif //PBRRENDERER_EDITORUI_HPP
