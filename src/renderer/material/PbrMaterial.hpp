//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_PBRMATERIAL_HPP
#define PBRRENDERER_PBRMATERIAL_HPP
#include "DeferredMaterial.hpp"
#include "Material.hpp"
#include "../core/Shader.hpp"
#include "../camera/Camera.hpp"
#include "../asset/Texture2D.hpp"
#include "../asset/Vertex.hpp"

namespace renderer {

    class PBRMaterial : public DeferredMaterial {
    public:
        PBRMaterial() = default;
        RenderPath renderPath() const noexcept override { return RenderPath::Deferred; }

        void setBaseColorFactor(const glm::vec4& value) noexcept { base_color_factor_ = value; }
        void setMetallicFactor(float value) noexcept { metallic_factor_ = value; }
        void setRoughnessFactor(float value) noexcept { roughness_factor_ = value; }
        void setNormalScale(float value) noexcept { normal_scale_ = value; }
        void setOcclusionStrength(float value) noexcept { occlusion_strength_ = value; }
        void setEmissiveFactor(const glm::vec3& value) noexcept { emissive_factor_ = value; }

        const glm::vec4& baseColorFactor() const noexcept { return base_color_factor_; }
        float metallicFactor() const noexcept { return metallic_factor_; }
        float roughnessFactor() const noexcept { return roughness_factor_; }
        float normalScale() const noexcept { return normal_scale_; }
        float occlusionStrength() const noexcept { return occlusion_strength_; }
        const glm::vec3& emissiveFactor() const noexcept { return emissive_factor_; }

        void setBaseColorMap(const Texture2D* texture) noexcept { base_color_map_ = texture; }
        void setMetallicRoughnessMap(const Texture2D* texture) noexcept { metallic_roughness_map_ = texture; }
        void setNormalMap(const Texture2D* texture) noexcept { normal_map_ = texture; }
        void setOcclusionMap(const Texture2D* texture) noexcept { occlusion_map_ = texture; }
        void setEmissiveMap(const Texture2D* texture) noexcept { emissive_map_ = texture; }

        const Texture2D* baseColorMap() const noexcept { return base_color_map_; }
        const Texture2D* metallicRoughnessMap() const noexcept { return metallic_roughness_map_; }
        const Texture2D* normalMap() const noexcept { return normal_map_; }
        const Texture2D* occlusionMap() const noexcept { return occlusion_map_; }
        const Texture2D* emissiveMap() const noexcept { return emissive_map_; }

        void setAlphaMode(AlphaMode mode) noexcept { alpha_mode_ = mode; }
        void setAlphaCutoff(float value) noexcept { alpha_cutoff_ = value; }
        void setDoubleSided(bool value) noexcept { double_sided_ = value; }

        AlphaMode alphaMode() const noexcept { return alpha_mode_; }
        float alphaCutoff() const noexcept { return alpha_cutoff_; }
        bool doubleSided() const noexcept { return double_sided_; }

    private:
        glm::vec4 base_color_factor_{1.0f, 1.0f, 1.0f, 1.0f};
        float metallic_factor_ = 1.0f;
        float roughness_factor_ = 1.0f;
        float normal_scale_ = 1.0f;
        float occlusion_strength_ = 1.0f;
        glm::vec3 emissive_factor_{0.0f, 0.0f, 0.0f};

        AlphaMode alpha_mode_ = AlphaMode::Opaque;
        float alpha_cutoff_ = 0.5f;
        bool double_sided_ = false;

        const Texture2D* base_color_map_ = nullptr;
        const Texture2D* metallic_roughness_map_ = nullptr;
        const Texture2D* normal_map_ = nullptr;
        const Texture2D* occlusion_map_ = nullptr;
        const Texture2D* emissive_map_ = nullptr;
    };
}


#endif //PBRRENDERER_PBRMATERIAL_HPP
