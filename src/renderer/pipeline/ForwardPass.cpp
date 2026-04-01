//
// Created by kaede on 2026/3/30.
//

#include "ForwardPass.hpp"
#include "../material/ForwardMaterial.hpp"

namespace renderer {

    void ForwardPass::execute(const std::vector<RenderItem>& forward_items,
                          const Camera& camera,
                          RenderContext& render_context,
                          const FrameBuffer& target_framebuffer)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "Forward Pass");

        target_framebuffer.bind();
        glViewport(0, 0, target_framebuffer.width(), target_framebuffer.height());
        glEnable(GL_DEPTH_TEST);

        render_context.camera_position = camera.position();

        for (const auto& item : forward_items) {
            if (!item.mesh || !item.material) {
                continue;
            }

            auto* material = dynamic_cast<const ForwardMaterial*>(item.material);
            if (!material) {
                continue;
            }

            material->bind(item.model_matrix, camera, render_context);
            item.mesh->draw();
        }

        target_framebuffer.unbind();
        glPopDebugGroup();
    }
} // renderer