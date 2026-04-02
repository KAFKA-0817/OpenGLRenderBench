//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_LIGHTINGPASS_HPP
#define PBRRENDERER_LIGHTINGPASS_HPP
#include "../../core/opengl.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"
#include "../asset/Model.hpp"
#include "../material/Material.hpp"

namespace renderer {

    class LightingPass {
    public:
        LightingPass(int width, int height);

        void resize(int width, int height);

        void execute(GLuint g_position,
                     GLuint g_normal,
                     GLuint g_albedo,
                     GLuint g_material,
                     GLuint g_emissive,
                     const RenderContext& context);

        const FrameBuffer& framebuffer() const noexcept { return framebuffer_; }
        GLuint colorAttachment() const noexcept { return framebuffer_.colorAttachment(0); }

        int width() const noexcept { return framebuffer_.width(); }
        int height() const noexcept { return framebuffer_.height(); }

        bool reloadShader();

    private:
        FrameBuffer framebuffer_;
        Mesh screen_quad_;
        Shader shader_;
    };
} // renderer

#endif //PBRRENDERER_LIGHTINGPASS_HPP