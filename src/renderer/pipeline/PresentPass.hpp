//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_PRESENTPASS_HPP
#define PBRRENDERER_PRESENTPASS_HPP
#include "../../core/opengl.hpp"
#include "../core/Shader.hpp"
#include "../asset/Model.hpp"
#include "RenderPass.hpp"

namespace renderer {
    class PresentPass :public RenderPass {
    public:
        PresentPass();

        void present(GLuint input_texture, int width, int height);

        bool reloadShader();
        Shader& shader() noexcept { return screen_shader_; }
        const Shader& shader() const noexcept { return screen_shader_; }

    private:
        Mesh screen_quad_;
        Shader screen_shader_;
    };
} // renderer

#endif //PBRRENDERER_PRESENTPASS_HPP