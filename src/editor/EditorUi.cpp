//
// Created by kaede on 2026/4/7.
//

#include "EditorUi.hpp"

#include <array>

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

namespace editor {
    namespace {
        constexpr glm::vec3 kDefaultDirectionalLightRotation{-70.0f, 33.0f, 0.0f};
        constexpr glm::vec3 kDefaultPointLightPosition{0.0f, 1.0f, 0.0f};

        TransformComponent makeDirectionalLightTransform() {
            TransformComponent transform{};
            transform.rotation = kDefaultDirectionalLightRotation;
            return transform;
        }

        TransformComponent makePointLightTransform() {
            TransformComponent transform{};
            transform.position = kDefaultPointLightPosition;
            return transform;
        }

        void ensureTransform(Scene& scene, Entity entity, const TransformComponent& default_transform = {}) {
            if (!scene.hasTransform(entity)) {
                scene.addTransform(entity, default_transform);
            }
        }

        void drawPreviewModeMenu(renderer::Renderer& renderer) {
            const auto current_mode = renderer.previewMode();

            if (ImGui::MenuItem("Final Scene", nullptr, current_mode == renderer::PreviewMode::FinalScene)) {
                renderer.setPreviewMode(renderer::PreviewMode::FinalScene);
            }
            if (ImGui::MenuItem("GPosition", nullptr, current_mode == renderer::PreviewMode::GPosition)) {
                renderer.setPreviewMode(renderer::PreviewMode::GPosition);
            }
            if (ImGui::MenuItem("GNormal", nullptr, current_mode == renderer::PreviewMode::GNormal)) {
                renderer.setPreviewMode(renderer::PreviewMode::GNormal);
            }
            if (ImGui::MenuItem("GAlbedo", nullptr, current_mode == renderer::PreviewMode::GAlbedo)) {
                renderer.setPreviewMode(renderer::PreviewMode::GAlbedo);
            }
            if (ImGui::MenuItem("GMaterial", nullptr, current_mode == renderer::PreviewMode::GMaterial)) {
                renderer.setPreviewMode(renderer::PreviewMode::GMaterial);
            }
            if (ImGui::MenuItem("GEmissive", nullptr, current_mode == renderer::PreviewMode::GEmissive)) {
                renderer.setPreviewMode(renderer::PreviewMode::GEmissive);
            }
        }
    }

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
            if (ImGui::BeginMenu("Render")) {
                if (ImGui::MenuItem("Reload Shaders")) {
                    commands_frame_.writable().reload_shaders = true;
                }

                if (ImGui::BeginMenu("Preview")) {
                    drawPreviewModeMenu(renderer_);
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

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
            if (ImGui::MenuItem("Create Directional Light")) {
                const Entity entity = scene_.createEntity();
                scene_.addName(entity, {"Directional Light"});
                scene_.addTransform(entity, makeDirectionalLightTransform());
                scene_.addDirectionalLight(entity, {});
                state_.selectedEntity = entity;
            }
            if (ImGui::MenuItem("Create Point Light")) {
                const Entity entity = scene_.createEntity();
                scene_.addName(entity, {"Point Light"});
                scene_.addTransform(entity, makePointLightTransform());
                scene_.addPointLight(entity, {});
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

            if (!scene_.hasDirectionalLight(entity)) {
                if (ImGui::MenuItem("Directional Light")) {
                    ensureTransform(scene_, entity, makeDirectionalLightTransform());
                    scene_.addDirectionalLight(entity, {});
                }
            }

            if (!scene_.hasPointLight(entity)) {
                if (ImGui::MenuItem("Point Light")) {
                    ensureTransform(scene_, entity, makePointLightTransform());
                    scene_.addPointLight(entity, {});
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
            float uniform_scale = transform->scale.x;
            if (ImGui::DragFloat("Scale", &uniform_scale, 0.05f)) {
                if (uniform_scale < 0.001f) {
                    uniform_scale = 0.001f;
                }
                transform->scale = glm::vec3(uniform_scale);
            }
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
            const auto ready_models = assetManager_.getReadyModels();

            std::string current_model_label = "None";
            for (const auto& entry : ready_models) {
                if (entry.model == mesh_renderer->model) {
                    current_model_label = entry.name;
                    break;
                }
            }
            if (mesh_renderer->model && current_model_label == "None") {
                current_model_label = "Bound";
            }
            if (ImGui::BeginCombo("Model", current_model_label.c_str())) {
                if (ImGui::Selectable("None", mesh_renderer->model == nullptr)) {
                    mesh_renderer->model = nullptr;
                }

                for (const auto& entry : ready_models) {
                    const bool selected = (entry.model == mesh_renderer->model);
                    if (ImGui::Selectable(entry.name.c_str(), selected)) {
                        mesh_renderer->model = entry.model;
                    }

                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            if (mesh_renderer->model) {
                ImGui::Text("Submeshes: %d", static_cast<int>(mesh_renderer->model->meshes().size()));
                ImGui::Text("Materials: %d", static_cast<int>(mesh_renderer->model->materials().size()));
            } else {
                ImGui::TextUnformatted("Model: null");
            }
        }

        if (auto* light = scene_.tryGetDirectionalLight(entity)) {
            ImGui::SeparatorText("Directional Light");
            ImGui::SameLine();
            if (ImGui::SmallButton("...##DirectionalLightComponent")) {
                ImGui::OpenPopup("DirectionalLightComponentMenu");
            }

            if (ImGui::BeginPopup("DirectionalLightComponentMenu")) {
                if (ImGui::MenuItem("Remove Component")) {
                    remove_requests.push_back(ComponentRemoveRequest::DirectionalLight);
                }
                ImGui::EndPopup();
            }

            ImGui::Checkbox("Enabled", &light->enabled);
            ImGui::ColorEdit3("Color", &light->color.x);
            ImGui::DragFloat("Intensity", &light->intensity, 0.05f, 0.0f, 100.0f);
            if (light->intensity < 0.0f) {
                light->intensity = 0.0f;
            }
            ImGui::TextDisabled("Direction follows Transform rotation.");
        }

        if (auto* light = scene_.tryGetPointLight(entity)) {
            ImGui::SeparatorText("Point Light");
            ImGui::SameLine();
            if (ImGui::SmallButton("...##PointLightComponent")) {
                ImGui::OpenPopup("PointLightComponentMenu");
            }

            if (ImGui::BeginPopup("PointLightComponentMenu")) {
                if (ImGui::MenuItem("Remove Component")) {
                    remove_requests.push_back(ComponentRemoveRequest::PointLight);
                }
                ImGui::EndPopup();
            }

            ImGui::Checkbox("Enabled", &light->enabled);
            ImGui::ColorEdit3("Color", &light->color.x);
            ImGui::DragFloat("Intensity##PointLight", &light->intensity, 0.05f, 0.0f, 100.0f);
            ImGui::DragFloat("Range", &light->range, 0.05f, 0.0f, 1000.0f);
            if (light->intensity < 0.0f) {
                light->intensity = 0.0f;
            }
            if (light->range < 0.0f) {
                light->range = 0.0f;
            }
            ImGui::TextDisabled("Position follows Transform position.");
        }

        for (const auto& request: remove_requests) {
            switch (request) {
                case ComponentRemoveRequest::Name:
                    scene_.removeName(entity);
                    break;
                case ComponentRemoveRequest::Transform:
                    scene_.removeTransform(entity);
                    break;
                case ComponentRemoveRequest::MeshRenderer:
                    scene_.removeMeshRenderer(entity);
                    break;
                case ComponentRemoveRequest::DirectionalLight:
                    scene_.removeDirectionalLight(entity);
                    break;
                case ComponentRemoveRequest::PointLight:
                    scene_.removePointLight(entity);
                    break;
                default:
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
