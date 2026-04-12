//
// Created by kaede on 2026/4/12.
//

#ifndef PBRRENDERER_BLOOMPASS_HPP
#define PBRRENDERER_BLOOMPASS_HPP

#include "../../core/noncopyable.hpp"
#include "../../core/opengl.hpp"
#include "../asset/Mesh.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"

namespace renderer {
    class BloomPass : public core::NonCopyable {
    public:
        BloomPass(int width, int height);

        void resize(int width, int height);
        void execute(GLuint scene_hdr_texture);

        GLuint colorAttachment() const noexcept { return composite_framebuffer_.colorAttachment(0); }
        bool reloadShader();

    private:
        FrameBuffer bright_framebuffer_;
        FrameBuffer ping_framebuffer_;
        FrameBuffer pong_framebuffer_;
        FrameBuffer composite_framebuffer_;
        Mesh screen_quad_;
        Shader bright_shader_;
        Shader blur_shader_;
        Shader composite_shader_;
    };
} // renderer

#endif //PBRRENDERER_BLOOMPASS_HPP
