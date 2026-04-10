//
// Created by kaede on 2026/4/10.
//

#include "OutlinePass.hpp"

#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"

namespace renderer {
    OutlinePass::OutlinePass(int width, int height)
        :framebuffer_(width,height),
        outline_shader_(core::ProjectPaths::shader("screen.vs"),core::ProjectPaths::shader("outline.fs")),
        screen_quad_(PrimitiveFactory::createQuad())
    {

    }

    void OutlinePass::execute(GLuint mask_attachment, GLuint scene_attachment) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Outline Pass");
        framebuffer_.bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        outline_shader_.use();
        outline_shader_.setInt("u_MaskTexture", 0);
        outline_shader_.setInt("u_SceneTexture", 1);
        outline_shader_.setFloat("u_TexelWidth", 1.0f / static_cast<float>(framebuffer_.width()));
        outline_shader_.setFloat("u_TexelHeight", 1.0f / static_cast<float>(framebuffer_.height()));
        outline_shader_.setInt("u_OutlineRadius", 1);
        outline_shader_.setVec3("u_OutlineColor", glm::vec3(1.0f, 0.72f, 0.22f));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mask_attachment);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, scene_attachment);

        screen_quad_.draw();

        framebuffer_.unbind();
        glEnable(GL_DEPTH_TEST);
        glPopDebugGroup();
    }
} // renderer
