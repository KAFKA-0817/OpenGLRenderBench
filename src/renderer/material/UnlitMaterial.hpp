//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_UNLITMATERIAL_HPP
#define PBRRENDERER_UNLITMATERIAL_HPP
#include "Material.hpp"
#include "../core/Shader.hpp"
#include "../asset/Texture2D.hpp"

namespace renderer {
    class UnlitMaterial: public Material {
    public:
        explicit UnlitMaterial(Shader& shader);

        void bind(const glm::mat4& model,
                  const Camera& camera,
                  const RenderContext& context) const override;

        RenderPath renderPath() const noexcept override { return RenderPath::Forward; }

        void setColor(const glm::vec3& color) noexcept { color_ = color; }
        const glm::vec3& color() const noexcept { return color_; }
        void setAlbedoMap(const Texture2D* texture) noexcept { albedo_map_ = texture; }
        const Texture2D* albedoMap() const noexcept { return albedo_map_; }

    private:
        glm::vec3 color_{1.0f, 1.0f, 1.0f};
        const Texture2D* albedo_map_ = nullptr;
    };
}


#endif //PBRRENDERER_UNLITMATERIAL_HPP
