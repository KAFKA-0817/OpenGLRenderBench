//
// Created by kaede on 2026/3/30.
//

#include "NormalPass.hpp"

#include "../../core/path.hpp"

namespace renderer {
    NormalPass::NormalPass(int width, int height)
    : framebuffer_(width, height),
      shader_(
          core::ProjectPaths::shader("normal.vs"),
          core::ProjectPaths::shader("normal.fs")) {}

    void NormalPass::resize(int width, int height) {
        framebuffer_.resize(width, height);
    }

    void NormalPass::execute(const std::vector<RenderItem>& items, const Camera& camera) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Normal Pass");

        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_.use();
        shader_.setMat4("u_View", camera.getViewMatrix());
        shader_.setMat4("u_Projection", camera.getProjectionMatrix());

        for (const auto& item : items) {
            if (!item.mesh) {
                continue;
            }

            shader_.setMat4("u_Model", item.model_matrix);
            item.mesh->draw();
        }

        framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool NormalPass::reloadShader() {
        return shader_.reload();
    }
} // renderer