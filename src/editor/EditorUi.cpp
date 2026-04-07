//
// Created by kaede on 2026/4/7.
//

#include "EditorUi.hpp"
#include "imgui.h"
#include "imgui_internal.h"

namespace editor {
    void EditorUI::draw() {
        drawDockSpace();
        if (state_.show_hierarchy) {
            drawHierarchy();
        }
        if (state_.show_inspector) {
            drawInspector();
        }
        if (state_.show_viewport) {
            drawViewport();
        }
    }

    void EditorUI::drawDockSpace() {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    }

    void EditorUI::drawHierarchy() {

    }

    void EditorUI::drawInspector() {

    }

    void EditorUI::drawViewport() {

    }
} // editor