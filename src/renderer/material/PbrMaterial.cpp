//
// Created by kaede on 2026/3/30.
//

#include "PbrMaterial.hpp"

namespace renderer {
    PBRMaterial::PBRMaterial(Shader& shader)
    :Material(shader) {}

    void PBRMaterial::bind(const glm::mat4& model,
                           const Camera& camera,
                           const RenderContext& context) const {
        shader_.use();

        shader_.setMat4("u_Model", model);
        shader_.setMat4("u_View", camera.getViewMatrix());
        shader_.setMat4("u_Projection", camera.getProjectionMatrix());

        shader_.setVec3("u_ViewPos", context.camera_position);
        shader_.setVec3("u_Light.direction", context.light_direction);
        shader_.setVec3("u_Light.color", context.light_color);

        shader_.setVec3("u_Material.albedoColor", albedo_);
        shader_.setFloat("u_Material.metallicFactor", metallic_);
        shader_.setFloat("u_Material.roughnessFactor", roughness_);
        shader_.setFloat("u_Material.aoFactor", ao_);

        shader_.setInt("u_Material.hasAlbedoMap", albedo_map_ ? 1 : 0);
        shader_.setInt("u_Material.hasMetallicMap", metallic_map_ ? 1 : 0);
        shader_.setInt("u_Material.hasRoughnessMap", roughness_map_ ? 1 : 0);
        shader_.setInt("u_Material.hasAoMap", ao_map_ ? 1 : 0);
        shader_.setInt("u_Material.hasNormalMap", normal_map_ ? 1 : 0);

        if (albedo_map_) {
            albedo_map_->bind(0);
            shader_.setInt("u_AlbedoMap", 0);
        }
        if (metallic_map_) {
            metallic_map_->bind(1);
            shader_.setInt("u_MetallicMap", 1);
        }
        if (roughness_map_) {
            roughness_map_->bind(2);
            shader_.setInt("u_RoughnessMap", 2);
        }
        if (ao_map_) {
            ao_map_->bind(3);
            shader_.setInt("u_AOMap", 3);
        }
        if (normal_map_) {
            normal_map_->bind(4);
            shader_.setInt("u_NormalMap", 4);
        }
    }
}