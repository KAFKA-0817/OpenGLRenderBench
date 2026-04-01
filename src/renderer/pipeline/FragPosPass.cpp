//
// Created by kaede on 2026/3/30.
//

#include "FragPosPass.hpp"

#include "../../core/path.hpp"

namespace renderer {
    FragPosPass::FragPosPass(int width, int height)
    : framebuffer_(width, height),
      shader_(
          core::ProjectPaths::shader("fragpos.vs"),
          core::ProjectPaths::shader("fragpos.fs")) {}

    void FragPosPass::resize(int width, int height) {
        framebuffer_.resize(width, height);
    }

    void FragPosPass::execute(const std::vector<RenderItem>& items,
                          const Camera& camera) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "FragPos Pass");

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

    bool FragPosPass::reloadShader() {
        return shader_.reload();
    }
} // renderer