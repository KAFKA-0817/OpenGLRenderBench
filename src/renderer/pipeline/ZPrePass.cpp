//
// Created by kaede on 2026/4/12.
//

#include "ZPrePass.hpp"

#include "../../core/path.hpp"
#include "../material/PbrMaterial.hpp"

namespace renderer {
    ZPrePass::ZPrePass(int width, int height)
        : framebuffer_(width, height),
          shader_(
              core::ProjectPaths::shader("z_pre.vs"),
              core::ProjectPaths::shader("z_pre.fs"))
    {
    }

    void ZPrePass::execute(const std::vector<RenderItem>& render_items, const Camera& camera) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 9, -1, "Z-Pre Pass");

        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glClear(GL_DEPTH_BUFFER_BIT);

        shader_.use();
        shader_.setMat4("u_View", camera.getViewMatrix());
        shader_.setMat4("u_Projection", camera.getProjectionMatrix());

        for (const auto& item : render_items) {
            if (!item.mesh) {
                continue;
            }

            if (const auto* pbr_material = dynamic_cast<const PBRMaterial*>(item.material)) {
                if (pbr_material->doubleSided()) {
                    glDisable(GL_CULL_FACE);
                } else {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                }
            } else {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }

            shader_.setMat4("u_Model", item.model_matrix);
            item.mesh->draw();
        }

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);
        framebuffer_.unbind();
        glPopDebugGroup();
    }
} // renderer
