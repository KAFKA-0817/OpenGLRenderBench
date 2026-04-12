//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_FORWARDMATERIAL_HPP
#define PBRRENDERER_FORWARDMATERIAL_HPP
#include "Material.hpp"
#include "../camera/Camera.hpp"
#include "../core/Shader.hpp"
#include "../../app/frame/RenderContextFrame.hpp"

namespace renderer {
    class ForwardMaterial : public Material {
    public:
        explicit ForwardMaterial(Shader& shader)
            : shader_(shader) {}

        ~ForwardMaterial() override = default;

        ForwardMaterial(ForwardMaterial&&) = delete;
        ForwardMaterial& operator=(ForwardMaterial&&) = delete;

        virtual void bind(const glm::mat4& model,
                          const Camera& camera,
                          const RenderContext& context,
                          GLuint shadow_map,
                          const glm::mat4& light_space_matrix) const = 0;

        Shader& shader() noexcept { return shader_; }
        const Shader& shader() const noexcept { return shader_; }

    protected:
        Shader& shader_;
    };
} // renderer

#endif //PBRRENDERER_FORWARDMATERIAL_HPP
