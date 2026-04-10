//
// Created by kaede on 2026/4/11.
//

#include "ShadowPass.hpp"

#include "../../core/path.hpp"
#include "../camera/OrthographicCamera.hpp"

namespace renderer {
    ShadowPass::ShadowPass(int width, int height)
        :frame_buffer_(width,height)
        ,shader_(core::ProjectPaths::shader("shadow_pass.vs"),core::ProjectPaths::shader("shadow_pass.fs"))
    {

    }

    void ShadowPass::setLightCamera(const glm::vec3& direction) {
        //todo set light view camera
    }

    void ShadowPass::execute(const std::vector<RenderItem>& render_items) const {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Shadow Pass");

        frame_buffer_.bind();
        glViewport(0,0,frame_buffer_.width(),frame_buffer_.height());
        glEnable(GL_DEPTH_TEST);
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        shader_.use();
        shader_.setMat4("u_lightSpaceMatrix",getLightSpaceMatrix());
        for (const auto& item : render_items) {
            shader_.setMat4("u_Model",item.model_matrix);
            item.mesh->draw();
        }
        frame_buffer_.unbind();
        glPopDebugGroup();
    }
} // renderer