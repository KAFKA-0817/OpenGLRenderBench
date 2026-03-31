//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_BLINNPHONGMATERIAL_HPP
#define PBRRENDERER_BLINNPHONGMATERIAL_HPP
#include "ForwardMaterial.hpp"
#include "../core/Shader.hpp"
#include "../camera/Camera.hpp"
#include "../asset/Texture2D.hpp"

namespace renderer {
    class BlinnPhongMaterial : public ForwardMaterial {
    public:
        BlinnPhongMaterial(Shader& shader);

        void bind(const glm::mat4& model,
                  const Camera& camera,
                  const RenderContext& context) const override;

        RenderPath renderPath() const noexcept override { return RenderPath::Forward; }

        void setAlbedo(const glm::vec3& albedo) noexcept { albedo_ = albedo; }
        void setSpecular(const glm::vec3& specular) noexcept { specular_ = specular; }
        void setShininess(float shininess) noexcept { shininess_ = shininess; }
        const glm::vec3& albedo() const noexcept { return albedo_; }
        const glm::vec3& specular() const noexcept { return specular_; }
        float shininess() const noexcept { return shininess_; }

        void setDiffuseMap(const Texture2D* texture) noexcept { diffuse_map_ = texture; }
        void setSpecularMap(const Texture2D* texture) noexcept { specular_map_ = texture; }
        const Texture2D* diffuseMap() const noexcept { return diffuse_map_; }
        const Texture2D* specularMap() const noexcept { return specular_map_; }

    private:
        glm::vec3 albedo_{1.0f, 1.0f, 1.0f};
        glm::vec3 specular_{0.5f, 0.5f, 0.5f};
        float shininess_ = 32.0f;

        const Texture2D* diffuse_map_ = nullptr;
        const Texture2D* specular_map_ = nullptr;
    };
}


#endif //PBRRENDERER_BLINNPHONGMATERIAL_HPP