//
// Created by kaede on 2026/4/19.
//

#include "SkyboxPass.hpp"

#include <glm/mat4x4.hpp>

#include "../../core/Log.hpp"
#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"

namespace renderer {
    SkyboxPass::SkyboxPass()
        : cube_(PrimitiveFactory::createCube()),
          shader_(
              core::ProjectPaths::shader("skybox.vs"),
              core::ProjectPaths::shader("skybox.fs"))
    {}

    void SkyboxPass::execute(const TextureCube& environment_map,
                             const Camera& camera,
                             const FrameBuffer& target_framebuffer,
                             const float lod) {
        if (environment_map.id() == 0) {
            return;
        }

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 10, -1, "Skybox Pass");

        target_framebuffer.bind();
        glViewport(0, 0, target_framebuffer.width(), target_framebuffer.height());
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        shader_.use();
        shader_.setMat4("u_View", glm::mat4(glm::mat3(camera.getViewMatrix())));
        shader_.setMat4("u_Projection", camera.getProjectionMatrix());
        environment_map.bind(0);
        shader_.setInt("u_EnvironmentMap", 0);
        shader_.setFloat("u_Lod", lod);
        cube_.draw();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        target_framebuffer.unbind();

        glPopDebugGroup();
    }

    bool SkyboxPass::reloadShader() {
        core::Log::getInstance().write("Shader", "Skybox shader reloaded");
        return shader_.reload();
    }
}
