//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_PBRMATERIAL_HPP
#define PBRRENDERER_PBRMATERIAL_HPP
#include "Material.hpp"
#include "../core/Shader.hpp"
#include "../camera/Camera.hpp"
#include "../asset/Texture2D.hpp"

namespace renderer {
    class PBRMaterial: public Material {
    public:
        PBRMaterial(Shader& shader);

        void bind(const glm::mat4& model,
                  const Camera& camera,
                  const RenderContext& context) const override;
        RenderPath renderPath() const noexcept override { return RenderPath::Deferred; }

        void setAlbedo(const glm::vec3& albedo) noexcept { albedo_ = albedo; }
        void setMetallic(float metallic) noexcept { metallic_ = metallic; }
        void setRoughness(float roughness) noexcept { roughness_ = roughness; }
        void setAo(float ao) noexcept { ao_ = ao; }
        const glm::vec3& albedo() const noexcept { return albedo_; }
        float metallic() const noexcept { return metallic_; }
        float roughness() const noexcept { return roughness_; }
        float ao() const noexcept { return ao_; }

        void setAlbedoMap(const Texture2D* texture) noexcept { albedo_map_ = texture; }
        void setMetallicMap(const Texture2D* texture) noexcept { metallic_map_ = texture; }
        void setRoughnessMap(const Texture2D* texture) noexcept { roughness_map_ = texture; }
        void setAoMap(const Texture2D* texture) noexcept { ao_map_ = texture; }
        void setNormalMap(const Texture2D* texture) noexcept { normal_map_ = texture; }
        const Texture2D* albedoMap() const noexcept { return albedo_map_; }
        const Texture2D* metallicMap() const noexcept { return metallic_map_; }
        const Texture2D* roughnessMap() const noexcept { return roughness_map_; }
        const Texture2D* aoMap() const noexcept { return ao_map_; }
        const Texture2D* normalMap() const noexcept { return normal_map_; }

    private:
        glm::vec3 albedo_{1.0f, 1.0f, 1.0f};
        float metallic_ = 0.0f;
        float roughness_ = 0.5f;
        float ao_ = 1.0f;

        const Texture2D* albedo_map_ = nullptr;
        const Texture2D* metallic_map_ = nullptr;
        const Texture2D* roughness_map_ = nullptr;
        const Texture2D* ao_map_ = nullptr;
        const Texture2D* normal_map_ = nullptr;
    };
}


#endif //PBRRENDERER_PBRMATERIAL_HPP
