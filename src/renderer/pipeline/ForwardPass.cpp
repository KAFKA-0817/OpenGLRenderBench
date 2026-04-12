//
// Created by kaede on 2026/3/30.
//

#include "ForwardPass.hpp"

#include <algorithm>
#include <string>

#include <glm/geometric.hpp>

#include "../../core/path.hpp"
#include "../material/ForwardMaterial.hpp"
#include "../material/PbrMaterial.hpp"

namespace renderer {
    namespace {
        void bindLights(Shader& shader, const RenderContext& context) {
            shader.setInt("u_HasDirectionalLight", context.directional_light.valid ? 1 : 0);
            shader.setInt("u_PointLightCount", static_cast<int>(context.point_light_count));

            if (context.directional_light.valid) {
                shader.setVec3("u_DirectionalLight.direction", context.directional_light.direction);
                shader.setVec3("u_DirectionalLight.color", context.directional_light.color);
                shader.setFloat("u_DirectionalLight.intensity", context.directional_light.intensity);
            }

            for (std::uint32_t i = 0; i < context.point_light_count; ++i) {
                const auto& light = context.point_lights[i];
                const std::string prefix = "u_PointLights[" + std::to_string(i) + "]";
                shader.setVec3(prefix + ".position", light.position);
                shader.setFloat(prefix + ".range", light.range);
                shader.setVec3(prefix + ".color", light.color);
                shader.setFloat(prefix + ".intensity", light.intensity);
            }
        }

        void bindTransparentPbrMaterial(Shader& shader,
                                        const PBRMaterial& material,
                                        const glm::mat4& model,
                                        const Camera& camera,
                                        const RenderContext& context) {
            shader.use();

            shader.setMat4("u_Model", model);
            shader.setMat4("u_View", camera.getViewMatrix());
            shader.setMat4("u_Projection", camera.getProjectionMatrix());

            shader.setVec3("u_ViewPos", context.camera_position);
            bindLights(shader, context);

            shader.setVec3("u_Material.baseColorFactor", glm::vec3(material.baseColorFactor()));
            shader.setFloat("u_Material.alphaFactor", material.baseColorFactor().a);
            shader.setFloat("u_Material.metallicFactor", material.metallicFactor());
            shader.setFloat("u_Material.roughnessFactor", material.roughnessFactor());
            shader.setFloat("u_Material.normalScale", material.normalScale());
            shader.setFloat("u_Material.occlusionStrength", material.occlusionStrength());
            shader.setVec3("u_Material.emissiveFactor", material.emissiveFactor());

            shader.setInt("u_Material.hasBaseColorMap", material.baseColorMap() ? 1 : 0);
            shader.setInt("u_Material.hasMetallicRoughnessMap", material.metallicRoughnessMap() ? 1 : 0);
            shader.setInt("u_Material.hasNormalMap", material.normalMap() ? 1 : 0);
            shader.setInt("u_Material.hasOcclusionMap", material.occlusionMap() ? 1 : 0);
            shader.setInt("u_Material.hasEmissiveMap", material.emissiveMap() ? 1 : 0);

            if (material.baseColorMap()) {
                material.baseColorMap()->bind(0);
                shader.setInt("u_BaseColorMap", 0);
            }
            if (material.metallicRoughnessMap()) {
                material.metallicRoughnessMap()->bind(1);
                shader.setInt("u_MetallicRoughnessMap", 1);
            }
            if (material.normalMap()) {
                material.normalMap()->bind(2);
                shader.setInt("u_NormalMap", 2);
            }
            if (material.occlusionMap()) {
                material.occlusionMap()->bind(3);
                shader.setInt("u_OcclusionMap", 3);
            }
            if (material.emissiveMap()) {
                material.emissiveMap()->bind(4);
                shader.setInt("u_EmissiveMap", 4);
            }
        }
    }

    ForwardPass::ForwardPass()
        : transparent_pbr_shader_(
            core::ProjectPaths::shader("forward_pbr.vs"),
            core::ProjectPaths::shader("forward_pbr.fs"))
    {}

    void ForwardPass::execute(const std::vector<RenderItem>& forward_items,
                              const Camera& camera,
                              const RenderContext& render_context,
                              GLuint shadow_map,
                              const glm::mat4& light_space_matrix,
                              const FrameBuffer& target_framebuffer)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "Forward Pass");

        target_framebuffer.bind();
        glViewport(0, 0, target_framebuffer.width(), target_framebuffer.height());
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        for (const auto& item : forward_items) {
            if (!item.mesh || !item.material) {
                continue;
            }

            auto* material = dynamic_cast<const ForwardMaterial*>(item.material);
            if (!material) {
                continue;
            }

            material->bind(item.model_matrix, camera, render_context, shadow_map, light_space_matrix);
            item.mesh->draw();
        }

        target_framebuffer.unbind();
        glPopDebugGroup();
    }

    void ForwardPass::executeTransparentPbr(const std::vector<RenderItem>& transparent_items,
                                            const Camera& camera,
                                            const RenderContext& render_context,
                                            const FrameBuffer& target_framebuffer) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 4, -1, "Transparent Forward PBR Pass");

        target_framebuffer.bind();
        glViewport(0, 0, target_framebuffer.width(), target_framebuffer.height());
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        std::vector<const RenderItem*> sorted_items;
        sorted_items.reserve(transparent_items.size());

        for (const auto& item : transparent_items) {
            const auto* material = dynamic_cast<const PBRMaterial*>(item.material);
            if (!item.mesh || !material || material->alphaMode() != AlphaMode::Blend) {
                continue;
            }
            sorted_items.push_back(&item);
        }

        const glm::vec3 camera_position = camera.position();
        std::sort(sorted_items.begin(), sorted_items.end(),
                  [&camera_position](const RenderItem* lhs, const RenderItem* rhs) {
                      const glm::vec3 lhs_position(lhs->model_matrix[3]);
                      const glm::vec3 rhs_position(rhs->model_matrix[3]);
                      const glm::vec3 lhs_delta = lhs_position - camera_position;
                      const glm::vec3 rhs_delta = rhs_position - camera_position;
                      return glm::dot(lhs_delta, lhs_delta) > glm::dot(rhs_delta, rhs_delta);
                  });

        for (const auto* item : sorted_items) {
            const auto* material = static_cast<const PBRMaterial*>(item->material);
            bindTransparentPbrMaterial(transparent_pbr_shader_,
                                       *material,
                                       item->model_matrix,
                                       camera,
                                       render_context);

            if (material->doubleSided()) {
                glDisable(GL_CULL_FACE);
            } else {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }

            item->mesh->draw();
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        target_framebuffer.unbind();
        glPopDebugGroup();
    }

    bool ForwardPass::reloadShader() {
        return transparent_pbr_shader_.reload();
    }
} // renderer
