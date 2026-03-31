//
// Created by kaede on 2026/3/30.
//

#include "GBufferPass.hpp"

#include "../../core/path.hpp"
#include "../material/PBRMaterial.hpp"

namespace renderer {
    GBufferPass::GBufferPass(int width, int height)
    : framebuffer_(width, height, 4, GL_RGBA16F),
      pbr_gbuffer_shader_(
          core::ProjectPaths::shader("gbuffer_pbr.vs"),
          core::ProjectPaths::shader("gbuffer_pbr.fs"))
    {}

    void GBufferPass::resize(int width, int height) {
        framebuffer_.resize(width, height);
    }

    void GBufferPass::execute(const std::vector<RenderItem>& deferred_items,
                          const Camera& camera) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "GBuffer Pass");

        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pbr_gbuffer_shader_.use();
        pbr_gbuffer_shader_.setMat4("u_View", camera.getViewMatrix());
        pbr_gbuffer_shader_.setMat4("u_Projection", camera.getProjectionMatrix());

        for (const auto& item : deferred_items) {
            if (!item.model || !item.material) {
                continue;
            }

            auto pbr = dynamic_cast<const PBRMaterial*>(item.material);
            if (!pbr) {
                continue;
            }

            pbr_gbuffer_shader_.setMat4("u_Model", item.model_matrix);

            pbr_gbuffer_shader_.setVec3("u_Material.albedoColor", pbr->albedo());
            pbr_gbuffer_shader_.setFloat("u_Material.metallicFactor", pbr->metallic());
            pbr_gbuffer_shader_.setFloat("u_Material.roughnessFactor", pbr->roughness());
            pbr_gbuffer_shader_.setFloat("u_Material.aoFactor", pbr->ao());

            pbr_gbuffer_shader_.setInt("u_Material.hasAlbedoMap", pbr->albedoMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasMetallicMap", pbr->metallicMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasRoughnessMap", pbr->roughnessMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasAoMap", pbr->aoMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasNormalMap", pbr->normalMap() ? 1 : 0);

            if (pbr->albedoMap()) {
                pbr->albedoMap()->bind(0);
                pbr_gbuffer_shader_.setInt("u_AlbedoMap", 0);
            }
            if (pbr->metallicMap()) {
                pbr->metallicMap()->bind(1);
                pbr_gbuffer_shader_.setInt("u_MetallicMap", 1);
            }
            if (pbr->roughnessMap()) {
                pbr->roughnessMap()->bind(2);
                pbr_gbuffer_shader_.setInt("u_RoughnessMap", 2);
            }
            if (pbr->aoMap()) {
                pbr->aoMap()->bind(3);
                pbr_gbuffer_shader_.setInt("u_AOMap", 3);
            }
            if (pbr->normalMap()) {
                pbr->normalMap()->bind(4);
                pbr_gbuffer_shader_.setInt("u_NormalMap", 4);
            }

            item.model->draw();
        }

        framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool GBufferPass::reloadShader() {
        return pbr_gbuffer_shader_.reload();
    }
} // renderer