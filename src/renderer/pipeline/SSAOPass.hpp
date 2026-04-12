//
// Created by kaede on 2026/4/11.
//

#ifndef PBRRENDERER_SSAOPASS_HPP
#define PBRRENDERER_SSAOPASS_HPP

#include <array>

#include "../../core/noncopyable.hpp"
#include "../../core/opengl.hpp"
#include "../asset/Mesh.hpp"
#include "../camera/Camera.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"

namespace renderer {
    class SSAOPass : public core::NonCopyable {
    public:
        SSAOPass(int width, int height);
        ~SSAOPass();
        SSAOPass(SSAOPass&& other) noexcept;
        SSAOPass& operator=(SSAOPass&& other) noexcept;

        void resize(int width, int height);

        void execute(GLuint g_position,
                     GLuint g_normal,
                     const Camera& camera);

        GLuint colorAttachment() const noexcept { return blur_framebuffer_.colorAttachment(0); }
        const FrameBuffer& rawFramebuffer() const noexcept { return raw_framebuffer_; }
        const FrameBuffer& blurFramebuffer() const noexcept { return blur_framebuffer_; }

        int width() const noexcept { return raw_framebuffer_.width(); }
        int height() const noexcept { return raw_framebuffer_.height(); }

        bool reloadShader();

    private:
        void destroyNoiseTexture() noexcept;
        void createNoiseTexture();
        void generateKernel();

    private:
        FrameBuffer raw_framebuffer_;
        FrameBuffer blur_framebuffer_;
        Mesh screen_quad_;
        Shader ssao_shader_;
        Shader blur_shader_;
        GLuint noise_texture_ = 0;
        std::array<glm::vec3, 32> kernel_{};
    };
} // renderer

#endif //PBRRENDERER_SSAOPASS_HPP
