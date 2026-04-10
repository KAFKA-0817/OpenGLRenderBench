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

    void MaskPass::execute(const RenderItem &item, const Camera &camera) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Mask Pass");
        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mask_shader_.use();
        mask_shader_.setMat4("u_View", camera.getViewMatrix());
        mask_shader_.setMat4("u_Projection", camera.getProjectionMatrix());
        mask_shader_.setMat4("u_Model",item.model_matrix);
        item.mesh->draw();
        framebuffer_.unbind();
        glDepthFunc(GL_LESS);
        glPopDebugGroup();
    }
} // renderer