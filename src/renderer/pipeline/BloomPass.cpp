//
// Created by kaede on 2026/4/12.
//

#include "BloomPass.hpp"

#include "../../core/Log.hpp"
#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"

namespace renderer {
    namespace {
        constexpr float kBloomThreshold = 1.0f;
        constexpr float kBloomStrength = 0.05f;
        constexpr int kBlurPassCount = 20; // 5 horizontal + 5 vertical
    }

    BloomPass::BloomPass(int width, int height)
        : bright_framebuffer_(width, height, 1, GL_RGBA16F),
          ping_framebuffer_(width, height, 1, GL_RGBA16F),
          pong_framebuffer_(width, height, 1, GL_RGBA16F),
          composite_framebuffer_(width, height, 1, GL_RGBA16F),
          screen_quad_(PrimitiveFactory::createQuad()),
          bright_shader_(
              core::ProjectPaths::shader("screen.vs"),
              core::ProjectPaths::shader("bloom_extract.fs")),
          blur_shader_(
              core::ProjectPaths::shader("screen.vs"),
              core::ProjectPaths::shader("bloom_blur.fs")),
          composite_shader_(
              core::ProjectPaths::shader("screen.vs"),
              core::ProjectPaths::shader("bloom_composite.fs")) {}

    void BloomPass::resize(int width, int height) {
        bright_framebuffer_.resize(width, height);
        ping_framebuffer_.resize(width, height);
        pong_framebuffer_.resize(width, height);
        composite_framebuffer_.resize(width, height);
    }

    void BloomPass::execute(GLuint scene_hdr_texture) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 9, -1, "Bloom Pass");

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        bright_framebuffer_.bind();
        glViewport(0, 0, bright_framebuffer_.width(), bright_framebuffer_.height());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        bright_shader_.use();
        bright_shader_.setInt("u_SceneTexture", 0);
        bright_shader_.setFloat("u_Threshold", kBloomThreshold);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scene_hdr_texture);
        screen_quad_.draw();

        bool horizontal = true;
        bool first_iteration = true;
        for (int i = 0; i < kBlurPassCount; ++i) {
            FrameBuffer& target = horizontal ? ping_framebuffer_ : pong_framebuffer_;
            target.bind();
            glViewport(0, 0, target.width(), target.height());
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            blur_shader_.use();
            blur_shader_.setInt("u_Image", 0);
            blur_shader_.setInt("u_Horizontal", horizontal ? 1 : 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,
                          first_iteration
                              ? bright_framebuffer_.colorAttachment(0)
                              : (horizontal ? pong_framebuffer_.colorAttachment(0)
                                            : ping_framebuffer_.colorAttachment(0)));

            screen_quad_.draw();

            horizontal = !horizontal;
            if (first_iteration) {
                first_iteration = false;
            }
        }

        const GLuint blurred_texture = horizontal
                                           ? ping_framebuffer_.colorAttachment(0)
                                           : pong_framebuffer_.colorAttachment(0);

        composite_framebuffer_.bind();
        glViewport(0, 0, composite_framebuffer_.width(), composite_framebuffer_.height());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        composite_shader_.use();
        composite_shader_.setInt("u_SceneTexture", 0);
        composite_shader_.setInt("u_BloomTexture", 1);
        composite_shader_.setFloat("u_BloomStrength", kBloomStrength);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scene_hdr_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blurred_texture);

        screen_quad_.draw();

        composite_framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool BloomPass::reloadShader() {
        const bool bright_ok = bright_shader_.reload();
        const bool blur_ok = blur_shader_.reload();
        const bool composite_ok = composite_shader_.reload();
        core::Log::getInstance().write("Shader","BloomShader Reloaded");
        return bright_ok && blur_ok && composite_ok;
    }
} // renderer
