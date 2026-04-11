//
// Created by kaede on 2026/4/11.
//

#include "ShadowPass.hpp"

#include "../../core/path.hpp"
#include "../camera/OrthographicCamera.hpp"
#include <glm/geometric.hpp>

namespace renderer {
    ShadowPass::ShadowPass(int width, int height)
        :frame_buffer_(width,height)
        ,shader_(core::ProjectPaths::shader("shadow_pass.vs"),core::ProjectPaths::shader("shadow_pass.fs"))
    {

    }

    void ShadowPass::setLightCamera(const Camera& view_camera, const glm::vec3& direction) {
        constexpr float kFocusDistance = 10.0f;
        constexpr float kHalfExtent = 20.0f;
        constexpr float kDepthExtent = 30.0f;
        constexpr float kNearPlane = 0.1f;

        const glm::vec3 light_dir = glm::normalize(direction);
        const glm::vec3 focus_center = view_camera.position() + view_camera.front() * kFocusDistance;
        const glm::vec3 light_position = focus_center - light_dir * kDepthExtent;

        glm::vec3 world_up(0.0f, 1.0f, 0.0f);
        if (glm::abs(glm::dot(light_dir, world_up)) > 0.99f) {
            world_up = glm::vec3(0.0f, 0.0f, 1.0f);
        }

        camera_ = OrthographicCamera(
            light_position,
            light_dir,
            world_up,
            -kHalfExtent,
            kHalfExtent,
            -kHalfExtent,
            kHalfExtent,
            kNearPlane,
            kDepthExtent * 2.0f
        );
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
