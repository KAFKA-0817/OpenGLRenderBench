//
// Created by kaede on 2026/4/7.
//

#ifndef PBRRENDERER_EDITORSTATE_HPP
#define PBRRENDERER_EDITORSTATE_HPP
#include <glm/vec2.hpp>

#include "scene/Entity.hpp"

namespace editor {
    struct EditorState {
        Entity selectedEntity = kInvalidEntity;

        bool show_hierarchy = true;
        bool show_inspector = true;
        bool show_viewport = true;

        glm::ivec2 viewport_size{900, 600};
        bool viewport_hovered = false;
        bool viewport_focused = false;
    };
}

#endif //PBRRENDERER_EDITORSTATE_HPP
