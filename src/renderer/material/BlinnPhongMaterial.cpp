//
// Created by kaede on 2026/3/30.
//

#include "BlinnPhongMaterial.hpp"

#include <string>

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
    }

    BlinnPhongMaterial::BlinnPhongMaterial(Shader& shader)
    : ForwardMaterial(shader)
    {}

    void BlinnPhongMaterial::bind(const glm::mat4& model,
                                  const Camera& camera,
                                  const RenderContext& context) const {
        shader_.use();

        shader_.setMat4("u_Model", model);
        shader_.setMat4("u_View", camera.getViewMatrix());
        shader_.setMat4("u_Projection", camera.getProjectionMatrix());

        shader_.setVec3("u_ViewPos", context.camera_position);
        bindLights(shader_, context);

        shader_.setVec3("u_Material.albedo", albedo_);
        shader_.setVec3("u_Material.specular", specular_);
        shader_.setFloat("u_Material.shininess", shininess_);

        shader_.setInt("u_Material.hasDiffuseMap", diffuse_map_ ? 1 : 0);
        shader_.setInt("u_Material.hasSpecularMap", specular_map_ ? 1 : 0);

        if (diffuse_map_) {
            diffuse_map_->bind(0);
            shader_.setInt("u_DiffuseMap", 0);
        }

        if (specular_map_) {
            specular_map_->bind(1);
            shader_.setInt("u_SpecularMap", 1);
        }
    }

}
