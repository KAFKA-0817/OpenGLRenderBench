//
// Created by kaede on 2026/3/30.
//

#include "UnlitMaterial.hpp"
#include "../camera/Camera.hpp"

namespace renderer {
    UnlitMaterial::UnlitMaterial(Shader& shader)
        :ForwardMaterial(shader)
    {

    }

    void UnlitMaterial::bind(const glm::mat4 &model, const Camera& camera, const RenderContext& context) const {
        (void)context;

        shader_.use();
        shader_.setMat4("u_Model", model);
        shader_.setMat4("u_View", camera.getViewMatrix());
        shader_.setMat4("u_Projection", camera.getProjectionMatrix());
        shader_.setVec3("u_Material.baseColor", color_);
        shader_.setInt("u_Material.hasAlbedoMap", albedo_map_ ? 1 : 0);

        if (albedo_map_) {
            albedo_map_->bind(0);
            shader_.setInt("u_AlbedoMap", 0);
        }
    }
}
