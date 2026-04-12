//
// Created by kaede on 2026/4/7.
//

#include "EditorUi.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "ImGuizmo.h"

namespace editor {
    namespace {
        constexpr glm::vec3 kDefaultDirectionalLightRotation{-70.0f, 33.0f, 0.0f};
        constexpr glm::vec3 kDefaultPointLightPosition{0.0f, 1.0f, 0.0f};
        constexpr float kDirectionalLightHelperLength = 1.8f;

        TransformComponent makeDirectionalLightTransform() {
            return TransformComponent::fromEulerDegrees(
                glm::vec3(0.0f, 0.0f, 0.0f),
                kDefaultDirectionalLightRotation
            );
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
            if (ImGui::MenuItem("SSAO", nullptr, current_mode == renderer::PreviewMode::SSAO)) {
                renderer.setPreviewMode(renderer::PreviewMode::SSAO);
            }
            if (ImGui::MenuItem("Mask", nullptr, current_mode == renderer::PreviewMode::Mask)) {
                renderer.setPreviewMode(renderer::PreviewMode::Mask);
            }
            if (ImGui::MenuItem("Shadow",nullptr,current_mode == renderer::PreviewMode::Shadow)) {
                renderer.setPreviewMode(renderer::PreviewMode::Shadow);
            }
        }

        const char* previewModeLabel(const renderer::PreviewMode mode) {
            switch (mode) {
                case renderer::PreviewMode::FinalScene:
                    return "Final Scene";
                case renderer::PreviewMode::GPosition:
                    return "GPosition";
                case renderer::PreviewMode::GNormal:
                    return "GNormal";
                case renderer::PreviewMode::GAlbedo:
                    return "GAlbedo";
                case renderer::PreviewMode::GMaterial:
                    return "GMaterial";
                case renderer::PreviewMode::GEmissive:
                    return "GEmissive";
                case renderer::PreviewMode::SSAO:
                    return "SSAO";
                case renderer::PreviewMode::Mask:
                    return "Mask";
                case renderer::PreviewMode::Shadow:
                    return "Shadow";
                default:
                    return "Final Scene";
            }
        }

        void drawPreviewModeCombo(renderer::Renderer& renderer) {
            const auto current_mode = renderer.previewMode();
            if (ImGui::BeginCombo("Preview", previewModeLabel(current_mode))) {
                constexpr std::array modes = {
                    renderer::PreviewMode::FinalScene,
                    renderer::PreviewMode::GPosition,
                    renderer::PreviewMode::GNormal,
                    renderer::PreviewMode::GAlbedo,
                    renderer::PreviewMode::GMaterial,
                    renderer::PreviewMode::GEmissive,
                    renderer::PreviewMode::SSAO,
                    renderer::PreviewMode::Mask,
                    renderer::PreviewMode::Shadow,
                };

                for (const auto mode : modes) {
                    const bool selected = (mode == current_mode);
                    if (ImGui::Selectable(previewModeLabel(mode), selected)) {
                        renderer.setPreviewMode(mode);
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }

        float sanitizeUniformScale(const float scale) {
            return std::max(scale, 0.001f);
        }

        float extractUniformScale(const glm::vec3& scale) {
            return sanitizeUniformScale((scale.x + scale.y + scale.z) / 3.0f);
        }

        glm::quat extractRotationQuaternion(const glm::mat4& matrix) {
            const glm::vec3 basis_x = glm::vec3(matrix[0]);
            const glm::vec3 basis_y = glm::vec3(matrix[1]);
            const glm::vec3 basis_z = glm::vec3(matrix[2]);

            const float scale_x = glm::length(basis_x);
            const float scale_y = glm::length(basis_y);
            const float scale_z = glm::length(basis_z);

            glm::mat3 rotation_matrix(1.0f);
            if (scale_x > 1e-6f) {
                rotation_matrix[0] = basis_x / scale_x;
            }
            if (scale_y > 1e-6f) {
                rotation_matrix[1] = basis_y / scale_y;
            }
            if (scale_z > 1e-6f) {
                rotation_matrix[2] = basis_z / scale_z;
            }

            return glm::normalize(glm::quat_cast(rotation_matrix));
        }

        ImGuizmo::OPERATION toImGuizmoOperation(const GizmoOperation operation) {
            switch (operation) {
                case GizmoOperation::Translate:
                    return ImGuizmo::TRANSLATE;
                case GizmoOperation::Rotate:
                    return ImGuizmo::ROTATE;
                case GizmoOperation::Scale:
                    return ImGuizmo::SCALEU;
                default:
                    return ImGuizmo::TRANSLATE;
            }
        }

        ImGuizmo::MODE toImGuizmoMode(const GizmoMode mode) {
            switch (mode) {
                case GizmoMode::Local:
                    return ImGuizmo::LOCAL;
                case GizmoMode::World:
                    return ImGuizmo::WORLD;
                default:
                    return ImGuizmo::LOCAL;
            }
        }

        void applyGizmoShortcuts(EditorState& state) {
            if (!state.viewport_focused || state.gizmo_using || ImGui::GetIO().WantTextInput) {
                return;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_W)) {
                state.gizmo_operation = GizmoOperation::Translate;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_E)) {
                state.gizmo_operation = GizmoOperation::Rotate;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_R)) {
                state.gizmo_operation = GizmoOperation::Scale;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Q)) {
                state.gizmo_mode = (state.gizmo_mode == GizmoMode::Local) ? GizmoMode::World : GizmoMode::Local;
            }
        }

        void drawGizmoToolbar(EditorState& state) {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Gizmo");
            ImGui::SameLine();
            if (ImGui::RadioButton("Translate", state.gizmo_operation == GizmoOperation::Translate)) {
                state.gizmo_operation = GizmoOperation::Translate;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", state.gizmo_operation == GizmoOperation::Rotate)) {
                state.gizmo_operation = GizmoOperation::Rotate;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", state.gizmo_operation == GizmoOperation::Scale)) {
                state.gizmo_operation = GizmoOperation::Scale;
            }

            ImGui::SameLine();
            ImGui::BeginDisabled(state.gizmo_operation == GizmoOperation::Scale);
            if (ImGui::RadioButton("Local", state.gizmo_mode == GizmoMode::Local)) {
                state.gizmo_mode = GizmoMode::Local;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("World", state.gizmo_mode == GizmoMode::World)) {
                state.gizmo_mode = GizmoMode::World;
            }
            ImGui::EndDisabled();
            ImGui::Separator();
        }

        void updateTransformFromGizmoMatrix(TransformComponent& transform, const glm::mat4& matrix) {
            const glm::vec3 basis_x = glm::vec3(matrix[0]);
            const glm::vec3 basis_y = glm::vec3(matrix[1]);
            const glm::vec3 basis_z = glm::vec3(matrix[2]);
            const float uniform_scale = sanitizeUniformScale((
                glm::length(basis_x) +
                glm::length(basis_y) +
                glm::length(basis_z)
            ) / 3.0f);

            transform.position = glm::vec3(matrix[3]);
            transform.rotation = extractRotationQuaternion(matrix);
            transform.scale = glm::vec3(uniform_scale);
        }

        void drawTransformGizmo(Scene& scene, const renderer::Camera& camera, EditorState& state) {
            state.gizmo_over = false;
            state.gizmo_using = false;

            if (state.selectedEntity == kInvalidEntity || !scene.isAlive(state.selectedEntity)) {
                return;
            }

            auto* transform = scene.tryGetTransform(state.selectedEntity);
            if (!transform) {
                return;
            }

            const glm::vec2 viewport_size = state.viewport_bounds_max - state.viewport_bounds_min;
            if (viewport_size.x <= 0.0f || viewport_size.y <= 0.0f) {
                return;
            }

            glm::mat4 model_matrix = transform->modelMatrix();
            const glm::mat4 view = camera.getViewMatrix();
            const glm::mat4 projection = camera.getProjectionMatrix();
            const ImGuizmo::OPERATION operation = toImGuizmoOperation(state.gizmo_operation);
            const ImGuizmo::MODE mode = (state.gizmo_operation == GizmoOperation::Scale)
                                            ? ImGuizmo::LOCAL
                                            : toImGuizmoMode(state.gizmo_mode);

            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(
                state.viewport_bounds_min.x,
                state.viewport_bounds_min.y,
                viewport_size.x,
                viewport_size.y
            );

            ImGuizmo::PushID(static_cast<int>(state.selectedEntity));
            const bool changed = ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                operation,
                mode,
                glm::value_ptr(model_matrix)
            );
            state.gizmo_over = ImGuizmo::IsOver();
            state.gizmo_using = ImGuizmo::IsUsing();
            ImGuizmo::PopID();

            if (changed) {
                updateTransformFromGizmoMatrix(*transform, model_matrix);
            }
        }

        void drawViewportFpsOverlay(const EditorState& state) {
            if (state.viewport_bounds_max.x <= state.viewport_bounds_min.x ||
                state.viewport_bounds_max.y <= state.viewport_bounds_min.y) {
                return;
            }

            char text[32];
            if (state.display_fps > 0.0f) {
                std::snprintf(text, sizeof(text), "FPS: %.1f", state.display_fps);
            } else {
                std::snprintf(text, sizeof(text), "FPS: --");
            }

            const ImVec2 text_position(
                state.viewport_bounds_min.x + 12.0f,
                state.viewport_bounds_min.y + 12.0f
            );
            const ImVec2 text_size = ImGui::CalcTextSize(text);
            const ImVec2 padding(8.0f, 5.0f);
            const ImVec2 background_min(text_position.x - padding.x, text_position.y - padding.y);
            const ImVec2 background_max(
                text_position.x + text_size.x + padding.x,
                text_position.y + text_size.y + padding.y
            );

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(background_min, background_max, IM_COL32(15, 18, 24, 180), 6.0f);
            draw_list->AddRect(background_min, background_max, IM_COL32(255, 255, 255, 40), 6.0f);
            draw_list->AddText(text_position, IM_COL32(235, 240, 245, 255), text);
        }

        bool projectWorldPointToViewport(
            const glm::vec3& world_point,
            const renderer::Camera& camera,
            const EditorState& state,
            ImVec2& screen_point
        ) {
            const glm::vec4 clip = camera.getProjectionMatrix() * camera.getViewMatrix() * glm::vec4(world_point, 1.0f);
            if (clip.w <= 1e-5f) {
                return false;
            }

            const glm::vec3 ndc = glm::vec3(clip) / clip.w;
            if (ndc.z < -1.0f || ndc.z > 1.0f) {
                return false;
            }

            const float width = state.viewport_bounds_max.x - state.viewport_bounds_min.x;
            const float height = state.viewport_bounds_max.y - state.viewport_bounds_min.y;
            screen_point.x = state.viewport_bounds_min.x + (ndc.x * 0.5f + 0.5f) * width;
            screen_point.y = state.viewport_bounds_min.y + (1.0f - (ndc.y * 0.5f + 0.5f)) * height;
            return true;
        }

        void drawSelectedDirectionalLightHelper(Scene& scene, const renderer::Camera& camera, const EditorState& state) {
            if (state.selectedEntity == kInvalidEntity || !scene.isAlive(state.selectedEntity)) {
                return;
            }

            if (!scene.hasDirectionalLight(state.selectedEntity)) {
                return;
            }

            const auto* transform = scene.tryGetTransform(state.selectedEntity);
            if (!transform) {
                return;
            }

            const glm::vec3 direction = transform->forwardDirection();
            const glm::vec3 end = transform->position;
            const glm::vec3 start = end - direction * kDirectionalLightHelperLength;

            ImVec2 start_screen{};
            ImVec2 end_screen{};
            if (!projectWorldPointToViewport(start, camera, state, start_screen) ||
                !projectWorldPointToViewport(end, camera, state, end_screen)) {
                return;
            }

            const ImU32 shaft_color = IM_COL32(255, 221, 87, 235);
            const ImU32 head_color = IM_COL32(255, 245, 195, 255);
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddLine(start_screen, end_screen, shaft_color, 2.5f);
            draw_list->AddCircleFilled(end_screen, 4.0f, head_color);

            const ImVec2 delta(end_screen.x - start_screen.x, end_screen.y - start_screen.y);
            const float delta_length_sq = delta.x * delta.x + delta.y * delta.y;
            if (delta_length_sq <= 1e-4f) {
                return;
            }

            const float inv_delta_length = 1.0f / std::sqrt(delta_length_sq);
            const ImVec2 dir(delta.x * inv_delta_length, delta.y * inv_delta_length);
            const ImVec2 perp(-dir.y, dir.x);
            const float head_length = 12.0f;
            const float head_half_width = 5.0f;
            const ImVec2 arrow_base(end_screen.x - dir.x * head_length, end_screen.y - dir.y * head_length);
            const ImVec2 left(arrow_base.x + perp.x * head_half_width, arrow_base.y + perp.y * head_half_width);
            const ImVec2 right(arrow_base.x - perp.x * head_half_width, arrow_base.y - perp.y * head_half_width);
            draw_list->AddTriangleFilled(end_screen, left, right, shaft_color);
        }
    }

    void EditorUI::draw() {
        ImGuizmo::BeginFrame();
        drawDockSpace();
        if (state_.show_renderer) {
            drawRendererPanel();
        }
        if (state_.show_hierarchy) {
            drawHierarchy();
        }
        if (state_.show_inspector) {
            drawInspector();
        }
        if (state_.show_viewport) {
            drawViewport();
        } else {
            state_.viewport_hovered = false;
            state_.viewport_focused = false;
            state_.viewport_content_hovered = false;
            state_.gizmo_over = false;
            state_.gizmo_using = false;
            state_.viewport_bounds_min = {0.0f, 0.0f};
            state_.viewport_bounds_max = {0.0f, 0.0f};
        }
        if (state_.show_console) {
            drawConsole();
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
            ImGuiID dock_left_top_id = dock_left_id;
            ImGuiID dock_left_bottom_id = ImGui::DockBuilderSplitNode(
                dock_left_id, ImGuiDir_Down, 0.5f, nullptr, &dock_left_top_id
            );
            ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(
                dock_main_id, ImGuiDir_Down, 0.33f, nullptr, &dock_main_id
            );
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(
                dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id
            );

            ImGui::DockBuilderDockWindow("Renderer", dock_left_top_id);
            ImGui::DockBuilderDockWindow("Hierarchy", dock_left_bottom_id);
            ImGui::DockBuilderDockWindow("Console", dock_bottom_id);
            ImGui::DockBuilderDockWindow("Viewport", dock_main_id);
            ImGui::DockBuilderDockWindow("Inspector", dock_right_id);

            ImGui::DockBuilderFinish(dockspace_id);
            dockspace_initialized_ = true;
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Window")) {
                ImGui::MenuItem("Renderer", nullptr, &state_.show_renderer);
                ImGui::MenuItem("Hierarchy", nullptr, &state_.show_hierarchy);
                ImGui::MenuItem("Inspector", nullptr, &state_.show_inspector);
                ImGui::MenuItem("Viewport", nullptr, &state_.show_viewport);
                ImGui::MenuItem("Console", nullptr, &state_.show_console);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorUI::drawRendererPanel() {
        if (!ImGui::Begin("Renderer", &state_.show_renderer)) {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Reload Shaders")) {
            commands_frame_.writable().reload_shaders = true;
        }

        bool ssao_enabled = renderer_.ssaoEnabled();
        if (ImGui::Checkbox("Enable SSAO", &ssao_enabled)) {
            renderer_.setSSAOEnabled(ssao_enabled);
        }

        bool bloom_enabled = renderer_.bloomEnabled();
        if (ImGui::Checkbox("Enable Bloom", &bloom_enabled)) {
            renderer_.setBloomEnabled(bloom_enabled);
        }

        drawPreviewModeCombo(renderer_);

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
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                state_.selectedEntity = entity;
                commands_frame_.writable().frame_selected = true;
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
            glm::vec3 rotation_degrees = transform->eulerDegrees();
            if (ImGui::DragFloat3("Rotation", &rotation_degrees.x, 0.5f)) {
                transform->setEulerDegrees(rotation_degrees);
            }
            float uniform_scale = extractUniformScale(transform->scale);
            if (ImGui::DragFloat("Scale", &uniform_scale, 0.05f)) {
                uniform_scale = sanitizeUniformScale(uniform_scale);
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
            state_.viewport_hovered = false;
            state_.viewport_focused = false;
            state_.viewport_content_hovered = false;
            state_.gizmo_over = false;
            state_.gizmo_using = false;
            ImGui::End();
            return;
        }

        state_.viewport_hovered = ImGui::IsWindowHovered();
        state_.viewport_focused = ImGui::IsWindowFocused();
        applyGizmoShortcuts(state_);
        drawGizmoToolbar(state_);

        const ImVec2 avail = ImGui::GetContentRegionAvail();
        state_.viewport_size = {
            std::max(0, static_cast<int>(avail.x)),
            std::max(0, static_cast<int>(avail.y))
        };

        const GLuint texture_id = renderer_.outputTexture();
        state_.viewport_content_hovered = false;
        state_.viewport_bounds_min = {0.0f, 0.0f};
        state_.viewport_bounds_max = {0.0f, 0.0f};

        if (texture_id != 0 && avail.x > 0.0f && avail.y > 0.0f) {
            ImGui::Image(
                texture_id,
                avail,
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );

            const ImVec2 image_min = ImGui::GetItemRectMin();
            const ImVec2 image_max = ImGui::GetItemRectMax();
            state_.viewport_bounds_min = {image_min.x, image_min.y};
            state_.viewport_bounds_max = {image_max.x, image_max.y};
            state_.viewport_content_hovered = ImGui::IsMouseHoveringRect(image_min, image_max);

            drawSelectedDirectionalLightHelper(scene_, camera_, state_);
            drawTransformGizmo(scene_, camera_, state_);
            drawViewportFpsOverlay(state_);
        } else {
            state_.gizmo_over = false;
            state_.gizmo_using = false;
            ImGui::TextUnformatted("No viewport image available.");
        }


        ImGui::End();
    }

    void EditorUI::drawConsole() {
        if (!ImGui::Begin("Console", &state_.show_console)) {
            ImGui::End();
            return;
        }

        const auto lines = core::Log::getInstance().snapshot();
        if (lines.empty()) {
            ImGui::TextUnformatted("No logs.");
            ImGui::End();
            return;
        }

        if (ImGui::Button("Clear")) {
            core::Log::getInstance().clear();
            ImGui::End();
            return;
        }

        ImGui::Separator();
        if (ImGui::BeginChild("ConsoleScrollRegion", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
            for (const auto& line : lines) {
                ImGui::TextUnformatted(line.c_str());
            }
        }
        ImGui::EndChild();
        ImGui::End();
    }
} // editor
