//
// Created by kaede on 2026/3/30.
//

#include "LightingPass.hpp"

#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"


namespace renderer {
    LightingPass::LightingPass(int width, int height)
    : framebuffer_(width, height, 1, GL_RGBA16F),
      screen_quad_(PrimitiveFactory::createQuad()),
      shader_(
          core::ProjectPaths::shader("lighting_pass.vs"),
          core::ProjectPaths::shader("lighting_pass.fs"))
    {}

    void LightingPass::resize(int width, int height) {
        framebuffer_.resize(width, height);
    }

    void LightingPass::execute(GLuint g_position,
                           GLuint g_normal,
                           GLuint g_albedo,
                           GLuint g_material,
                           const RenderContext& context) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Lighting Pass");

        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader_.use();

        shader_.setVec3("u_ViewPos", context.camera_position);
        shader_.setVec3("u_Light.direction", context.light_direction);
        shader_.setVec3("u_Light.color", context.light_color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        shader_.setInt("u_GPosition", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        shader_.setInt("u_GNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albedo);
        shader_.setInt("u_GAlbedo", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, g_material);
        shader_.setInt("u_GMaterial", 3);

        screen_quad_.draw();

        framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool LightingPass::reloadShader() {
        return shader_.reload();
    }
} // renderer