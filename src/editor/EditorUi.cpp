//
// Created by kaede on 2026/4/7.
//

#include "EditorUi.hpp"

#include <array>

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

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
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        window_flags |= ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("EditorDockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        if (!dockspace_initialized_) {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

            ImGuiID dock_main_id = dockspace_id;
            ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(
                dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id
            );
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(
                dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id
            );

            ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
            ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
            ImGui::DockBuilderDockWindow("Viewport", dock_main_id);

            ImGui::DockBuilderFinish(dockspace_id);
            dockspace_initialized_ = true;
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Window")) {
                ImGui::MenuItem("Hierarchy", nullptr, &state_.show_hierarchy);
                ImGui::MenuItem("Inspector", nullptr, &state_.show_inspector);
                ImGui::MenuItem("Viewport", nullptr, &state_.show_viewport);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorUI::drawHierarchy() {
        if (!ImGui::Begin("Hierarchy", &state_.show_hierarchy)) {
            ImGui::End();
            return;
        }

        //右键菜单
        if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (ImGui::MenuItem("Create Empty")) {
                const Entity entity = scene_.createEntity();
                scene_.addName(entity, {"Empty"});
                scene_.addTransform(entity, {});
                state_.selectedEntity = entity;
            }
            ImGui::EndPopup();
        }

        Entity entity_to_delete = kInvalidEntity;
        Entity entity_to_rename = kInvalidEntity;
        for (Entity entity : scene_.entities()) {
            std::string label = "Entity " + std::to_string(entity);

            if (const auto* name = scene_.tryGetName(entity)) {
                if (!name->name.empty()) {
                    label = name->name + "##" + std::to_string(entity);
                }
            }

            const bool selected = (state_.selectedEntity == entity);
            if (ImGui::Selectable(label.c_str(), selected)) {
                state_.selectedEntity = entity;
            }

            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Rename")) {
                    entity_to_rename = entity;
                }

                if (ImGui::MenuItem("Delete")) {
                    entity_to_delete = entity;
                }

                ImGui::EndPopup();
            }
        }

        if (entity_to_rename != kInvalidEntity) {
            rename_target_ = entity_to_rename;

            if (auto* name = scene_.tryGetName(entity_to_rename)) {
                rename_buffer_ = name->name;
            } else {
                rename_buffer_ = "Entity";
            }

            ImGui::OpenPopup("Rename Entity");
        }

        if (ImGui::BeginPopupModal("Rename Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted("Rename selected entity:");
            ImGui::InputText("##RenameEntityInput", &rename_buffer_);

            if (ImGui::Button("OK")) {
                if (rename_target_ != kInvalidEntity && scene_.isAlive(rename_target_)) {
                    if (auto* name = scene_.tryGetName(rename_target_)) {
                        name->name = rename_buffer_;
                    } else {
                        scene_.addName(rename_target_, {rename_buffer_});
                    }
                }

                rename_target_ = kInvalidEntity;
                rename_buffer_.clear();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel")) {
                rename_target_ = kInvalidEntity;
                rename_buffer_.clear();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (entity_to_delete != kInvalidEntity && scene_.isAlive(entity_to_delete)) {
            scene_.destroyEntity(entity_to_delete);

            if (state_.selectedEntity == entity_to_delete) {
                state_.selectedEntity = kInvalidEntity;
            }

            if (rename_target_ == entity_to_delete) {
                rename_target_ = kInvalidEntity;
                rename_buffer_.clear();
            }
        }

        ImGui::End();
    }

    void EditorUI::drawInspector() {
        if (!ImGui::Begin("Inspector", &state_.show_inspector)) {
            ImGui::End();
            return;
        }

        const Entity entity = state_.selectedEntity;
        if (entity == kInvalidEntity || !scene_.isAlive(entity)) {
            ImGui::TextUnformatted("Nothing selected.");
            ImGui::End();
            return;
        }

        ImGui::Text("Entity: %u", entity);
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponentPopup");
        }
        if (ImGui::BeginPopup("AddComponentPopup")) {
            if (!scene_.hasName(entity)) {
                if (ImGui::MenuItem("Name")) {
                    scene_.addName(entity, {"Entity"});
                }
            }

            if (!scene_.hasTransform(entity)) {
                if (ImGui::MenuItem("Transform")) {
                    scene_.addTransform(entity, {});
                }
            }

            if (!scene_.hasMeshRenderer(entity)) {
                if (ImGui::MenuItem("MeshRenderer")) {
                    scene_.addMeshRenderer(entity, {});
                }
            }

            ImGui::EndPopup();
        }

        std::vector<ComponentRemoveRequest> remove_requests;
        if (auto* name = scene_.tryGetName(entity)) {
            ImGui::SeparatorText("Name");
            ImGui::InputText("Name", &name->name);
        }

        if (auto* transform = scene_.tryGetTransform(entity)) {
            ImGui::SeparatorText("Transform");
            ImGui::SameLine();
            if (ImGui::SmallButton("...##TransformComponent")) {
                ImGui::OpenPopup("TransformComponentMenu");
            }

            if (ImGui::BeginPopup("TransformComponentMenu")) {
                if (ImGui::MenuItem("Remove Component")) {
                    remove_requests.push_back(ComponentRemoveRequest::Transform);
                }
                ImGui::EndPopup();
            }

            ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform->rotation.x, 0.5f);
            ImGui::DragFloat3("Scale", &transform->scale.x, 0.05f);

            if (transform->scale.x < 0.001f) transform->scale.x = 0.001f;
            if (transform->scale.y < 0.001f) transform->scale.y = 0.001f;
            if (transform->scale.z < 0.001f) transform->scale.z = 0.001f;
        }

        if (auto* mesh_renderer = scene_.tryGetMeshRenderer(entity)) {
            ImGui::SeparatorText("MeshRenderer");
            ImGui::SameLine();
            if (ImGui::SmallButton("...##MeshRendererComponent")) {
                ImGui::OpenPopup("MeshRendererComponentMenu");
            }

            if (ImGui::BeginPopup("MeshRendererComponentMenu")) {
                if (ImGui::MenuItem("Remove Component")) {
                    remove_requests.push_back(ComponentRemoveRequest::MeshRenderer);
                }
                ImGui::EndPopup();
            }
            ImGui::Checkbox("Visible", &mesh_renderer->visible);

            if (mesh_renderer->model) {
                ImGui::Text("Model: bound");
                ImGui::Text("Submeshes: %d", static_cast<int>(mesh_renderer->model->meshes().size()));
                ImGui::Text("Materials: %d", static_cast<int>(mesh_renderer->model->materials().size()));
            } else {
                ImGui::TextUnformatted("Model: null");
            }
        }

        for (const auto& request: remove_requests) {
            switch (request) {
                default:
                case ComponentRemoveRequest::Transform:
                    scene_.removeTransform(entity);
                    break;
                case ComponentRemoveRequest::MeshRenderer:
                    scene_.removeMeshRenderer(entity);
                    break;
                case ComponentRemoveRequest::Name:
                    scene_.removeName(entity);
                    break;
            }
        }

        ImGui::End();
    }

    void EditorUI::drawViewport() {
        if (!ImGui::Begin("Viewport", &state_.show_viewport)) {
            ImGui::End();
            return;
        }

        const ImVec2 avail = ImGui::GetContentRegionAvail();
        state_.viewport_size = {
            static_cast<int>(avail.x),
            static_cast<int>(avail.y)
        };
        state_.viewport_hovered = ImGui::IsWindowHovered();
        state_.viewport_focused = ImGui::IsWindowFocused();

        const GLuint texture_id = renderer_.outputTexture();

        if (texture_id != 0 && avail.x > 0.0f && avail.y > 0.0f) {
            ImGui::Image(
                texture_id,
                avail,
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );
        } else {
            ImGui::TextUnformatted("No viewport image available.");
        }


        ImGui::End();
    }
} // editor