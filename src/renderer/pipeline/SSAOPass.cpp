//
// Created by kaede on 2026/4/11.
//

#include "SSAOPass.hpp"

#include <random>
#include <string>
#include <vector>

#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"

namespace renderer {
    namespace {
        constexpr int kNoiseSize = 4;
        constexpr float kRadius = 0.6f;
        constexpr float kBias = 0.025f;
    }

    SSAOPass::SSAOPass(int width, int height)
        : raw_framebuffer_(width, height, 1, GL_RGBA16F),
          blur_framebuffer_(width, height, 1, GL_RGBA16F),
          screen_quad_(PrimitiveFactory::createQuad()),
          ssao_shader_(
              core::ProjectPaths::shader("screen.vs"),
              core::ProjectPaths::shader("ssao.fs")),
          blur_shader_(
              core::ProjectPaths::shader("screen.vs"),
              core::ProjectPaths::shader("ssao_blur.fs")) {
        generateKernel();
        createNoiseTexture();
    }

    SSAOPass::~SSAOPass() {
        destroyNoiseTexture();
    }

    SSAOPass::SSAOPass(SSAOPass&& other) noexcept
        : raw_framebuffer_(std::move(other.raw_framebuffer_)),
          blur_framebuffer_(std::move(other.blur_framebuffer_)),
          screen_quad_(std::move(other.screen_quad_)),
          ssao_shader_(std::move(other.ssao_shader_)),
          blur_shader_(std::move(other.blur_shader_)),
          noise_texture_(other.noise_texture_),
          kernel_(other.kernel_) {
        other.noise_texture_ = 0;
    }

    SSAOPass& SSAOPass::operator=(SSAOPass&& other) noexcept {
        if (this != &other) {
            destroyNoiseTexture();

            raw_framebuffer_ = std::move(other.raw_framebuffer_);
            blur_framebuffer_ = std::move(other.blur_framebuffer_);
            screen_quad_ = std::move(other.screen_quad_);
            ssao_shader_ = std::move(other.ssao_shader_);
            blur_shader_ = std::move(other.blur_shader_);
            noise_texture_ = other.noise_texture_;
            kernel_ = other.kernel_;

            other.noise_texture_ = 0;
        }
        return *this;
    }

    void SSAOPass::resize(int width, int height) {
        raw_framebuffer_.resize(width, height);
        blur_framebuffer_.resize(width, height);
    }

    void SSAOPass::execute(GLuint g_position,
                           GLuint g_normal,
                           const Camera& camera) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 8, -1, "SSAO Pass");

        raw_framebuffer_.bind();
        glViewport(0, 0, raw_framebuffer_.width(), raw_framebuffer_.height());
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ssao_shader_.use();
        ssao_shader_.setMat4("u_View", camera.getViewMatrix());
        ssao_shader_.setMat4("u_Projection", camera.getProjectionMatrix());
        ssao_shader_.setFloat("u_Radius", kRadius);
        ssao_shader_.setFloat("u_Bias", kBias);
        ssao_shader_.setFloat("u_NoiseScaleX", static_cast<float>(raw_framebuffer_.width()) / static_cast<float>(kNoiseSize));
        ssao_shader_.setFloat("u_NoiseScaleY", static_cast<float>(raw_framebuffer_.height()) / static_cast<float>(kNoiseSize));

        for (std::size_t i = 0; i < kernel_.size(); ++i) {
            ssao_shader_.setVec3("u_Samples[" + std::to_string(i) + "]", kernel_[i]);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        ssao_shader_.setInt("u_GPosition", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        ssao_shader_.setInt("u_GNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noise_texture_);
        ssao_shader_.setInt("u_NoiseTexture", 2);

        screen_quad_.draw();

        blur_framebuffer_.bind();
        glViewport(0, 0, blur_framebuffer_.width(), blur_framebuffer_.height());
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        blur_shader_.use();
        blur_shader_.setFloat("u_TexelSizeX", 1.0f / static_cast<float>(blur_framebuffer_.width()));
        blur_shader_.setFloat("u_TexelSizeY", 1.0f / static_cast<float>(blur_framebuffer_.height()));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, raw_framebuffer_.colorAttachment(0));
        blur_shader_.setInt("u_InputTexture", 0);

        screen_quad_.draw();

        blur_framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool SSAOPass::reloadShader() {
        const bool ssao_ok = ssao_shader_.reload();
        const bool blur_ok = blur_shader_.reload();
        return ssao_ok && blur_ok;
    }

    void SSAOPass::destroyNoiseTexture() noexcept {
        if (noise_texture_ != 0) {
            glDeleteTextures(1, &noise_texture_);
            noise_texture_ = 0;
        }
    }

    void SSAOPass::createNoiseTexture() {
        destroyNoiseTexture();

        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        std::vector<float> noise_data(kNoiseSize * kNoiseSize * 4);
        for (int i = 0; i < kNoiseSize * kNoiseSize; ++i) {
            noise_data[i * 4 + 0] = dist(rng);
            noise_data[i * 4 + 1] = dist(rng);
            noise_data[i * 4 + 2] = 0.0f;
            noise_data[i * 4 + 3] = 1.0f;
        }

        glGenTextures(1, &noise_texture_);
        glBindTexture(GL_TEXTURE_2D, noise_texture_);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA16F,
                     kNoiseSize,
                     kNoiseSize,
                     0,
                     GL_RGBA,
                     GL_FLOAT,
                     noise_data.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void SSAOPass::generateKernel() {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

        for (std::size_t i = 0; i < kernel_.size(); ++i) {
            glm::vec3 sample(
                dist01(rng) * 2.0f - 1.0f,
                dist01(rng) * 2.0f - 1.0f,
                dist01(rng));
            sample = glm::normalize(sample);
            sample *= dist01(rng);

            const float t = static_cast<float>(i) / static_cast<float>(kernel_.size());
            const float scale = 0.1f + (t * t) * 0.9f;
            kernel_[i] = sample * scale;
        }
    }
} // renderer
