//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_PRESENTPASS_HPP
#define PBRRENDERER_PRESENTPASS_HPP
#include "../../core/opengl.hpp"
#include "../core/Shader.hpp"
#include "../asset/Model.hpp"
#include "RenderPass.hpp"
#include "../core/FrameBuffer.hpp"

namespace renderer {
    class PresentPass :public RenderPass {
    public:
        PresentPass(int width, int height);

        void present(GLuint input_texture, float hdrExposure) const;
        GLuint colorAttachment() const noexcept { return framebuffer_.colorAttachment(0); }
        void resize(int width, int height) { framebuffer_.resize(width, height); }
        bool reloadShader();

        Shader& shader() noexcept { return screen_shader_; }
        const Shader& shader() const noexcept { return screen_shader_; }

    private:
        Mesh screen_quad_;
        Shader screen_shader_;
        FrameBuffer framebuffer_;
    };
} // renderer

#endif //PBRRENDERER_PRESENTPASS_HPP