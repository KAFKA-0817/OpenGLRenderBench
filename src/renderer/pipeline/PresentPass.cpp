//
// Created by kaede on 2026/3/30.
//

#include "PresentPass.hpp"

#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"

namespace renderer {
    PresentPass::PresentPass(int width, int height)
    : screen_quad_(PrimitiveFactory::createQuad()),
      screen_shader_(
          core::ProjectPaths::shader("screen.vs"),
          core::ProjectPaths::shader("screen.fs")),
    framebuffer_(width, height)
    {}

    void PresentPass::present(GLuint input_texture, float hdrExposure) const {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Present Pass");

        framebuffer_.bind();
        glDisable(GL_DEPTH_TEST);

        screen_shader_.use();
        screen_shader_.setInt("u_ScreenTexture", 0);
        screen_shader_.setFloat("hdr_exposure", hdrExposure);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input_texture);

        screen_quad_.draw();
        framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool PresentPass::reloadShader() {
        return screen_shader_.reload();
    }
} // renderer