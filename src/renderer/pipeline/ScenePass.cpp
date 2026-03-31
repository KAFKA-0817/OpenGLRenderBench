//
// Created by kaede on 2026/3/30.
//

#include "ScenePass.hpp"

namespace renderer {

    ScenePass::ScenePass(int width, int height)
    : framebuffer_(width, height) {}

    void ScenePass::resize(int width, int height) {
        framebuffer_.resize(width, height);
    }

    void ScenePass::execute(const std::vector<RenderItem>& items,
                        const Camera& camera,
                        RenderContext& render_context,
                        float clear_r,
                        float clear_g,
                        float clear_b,
                        float clear_a)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Scene Pass");

        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glEnable(GL_DEPTH_TEST);
        glClearColor(clear_r, clear_g, clear_b, clear_a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_context.camera_position = camera.position();

        for (const auto& item : items) {
            if (!item.model || !item.material) {
                continue;
            }

            item.material->bind(item.model_matrix, camera, render_context);
            item.model->draw();
        }

        framebuffer_.unbind();
        glPopDebugGroup();
    }

} // renderer