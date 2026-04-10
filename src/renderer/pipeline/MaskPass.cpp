//
// Created by kaede on 2026/4/10.
//

#include "MaskPass.hpp"

#include "../../core/path.hpp"

namespace renderer {
    MaskPass::MaskPass(int width, int height)
        :framebuffer_(width,height),
        mask_shader_(core::ProjectPaths::shader("mask.vs"),core::ProjectPaths::shader("mask.fs"))
    {

    }

    void MaskPass::execute(const std::vector<RenderItem>& items, const Camera &camera) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Mask Pass");
        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);

        mask_shader_.use();
        mask_shader_.setMat4("u_View", camera.getViewMatrix());
        mask_shader_.setMat4("u_Projection", camera.getProjectionMatrix());

        for (const auto& item : items) {
            mask_shader_.setMat4("u_Model",item.model_matrix);
            item.mesh->draw();
        }

        framebuffer_.unbind();
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.0f, 0.0f);
        glPopDebugGroup();
    }
} // renderer