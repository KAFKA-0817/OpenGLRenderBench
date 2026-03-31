//
// Created by kaede on 2026/3/30.
//

#include "BlinnPhongMaterial.hpp"

namespace renderer {
    BlinnPhongMaterial::BlinnPhongMaterial(Shader& shader)
    : Material(shader)
    {}

    void BlinnPhongMaterial::bind(const glm::mat4& model,
                                  const Camera& camera,
                                  const RenderContext& context) const {
        shader_.use();

        shader_.setMat4("u_Model", model);
        shader_.setMat4("u_View", camera.getViewMatrix());
        shader_.setMat4("u_Projection", camera.getProjectionMatrix());

        shader_.setVec3("u_ViewPos", context.camera_position);
        shader_.setVec3("u_Light.direction", context.light_direction);
        shader_.setVec3("u_Light.color", context.light_color);

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