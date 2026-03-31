//
// Created by kaede on 2026/3/30.
//

#include "PresentPass.hpp"

#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"

namespace renderer {
    PresentPass::PresentPass()
    : screen_quad_(PrimitiveFactory::createQuad()),
      screen_shader_(
          core::ProjectPaths::shader("screen.vs"),
          core::ProjectPaths::shader("screen.fs"))
    {}

    void PresentPass::present(GLuint input_texture, int width, int height) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Present Pass");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glDisable(GL_DEPTH_TEST);

        screen_shader_.use();
        screen_shader_.setInt("u_ScreenTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input_texture);

        screen_quad_.draw();

        glPopDebugGroup();
    }

    bool PresentPass::reloadShader() {
        return screen_shader_.reload();
    }
} // renderer