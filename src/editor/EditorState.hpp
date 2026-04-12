//
// Created by kaede on 2026/4/7.
//

#ifndef PBRRENDERER_EDITORSTATE_HPP
#define PBRRENDERER_EDITORSTATE_HPP
#include <glm/vec2.hpp>

#include "scene/Entity.hpp"

namespace editor {
    enum class GizmoOperation {
        Translate,
        Rotate,
        Scale
    };

    enum class GizmoMode {
        Local,
        World
    };

    struct EditorState {
        Entity selectedEntity = kInvalidEntity;

        bool show_hierarchy = true;
        bool show_renderer = true;
        bool show_inspector = true;
        bool show_viewport = true;
        bool show_console = true;

        glm::ivec2 viewport_size{900, 600};
        glm::vec2 viewport_bounds_min{0.0f, 0.0f};
        glm::vec2 viewport_bounds_max{0.0f, 0.0f};
        bool viewport_hovered = false;
        bool viewport_focused = false;
        bool viewport_content_hovered = false;
        bool gizmo_over = false;
        bool gizmo_using = false;
        float display_fps = 0.0f;

        GizmoOperation gizmo_operation = GizmoOperation::Translate;
        GizmoMode gizmo_mode = GizmoMode::Local;
    };
}

#endif //PBRRENDERER_EDITORSTATE_HPP
