//
// Created by kaede on 2026/3/30.
//

#include "GBufferPass.hpp"

#include "../../core/path.hpp"
#include "../material/PBRMaterial.hpp"

namespace renderer {
    GBufferPass::GBufferPass(int width, int height)
    : framebuffer_(width, height, 5, GL_RGBA16F),
      pbr_gbuffer_shader_(
          core::ProjectPaths::shader("gbuffer_pbr.vs"),
          core::ProjectPaths::shader("gbuffer_pbr.fs"))
    {}

    void GBufferPass::resize(int width, int height) {
        framebuffer_.resize(width, height);
    }

    void GBufferPass::execute(const std::vector<RenderItem>& deferred_items, const Camera& camera)
    {
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
            if (!item.mesh || !item.material) {
                continue;
            }

            auto pbr = dynamic_cast<const PBRMaterial*>(item.material);
            if (!pbr) {
                continue;
            }

            pbr_gbuffer_shader_.setMat4("u_Model", item.model_matrix);

            pbr_gbuffer_shader_.setVec3("u_Material.baseColorFactor", glm::vec3(pbr->baseColorFactor()));
            pbr_gbuffer_shader_.setFloat("u_Material.alphaFactor", pbr->baseColorFactor().a);
            pbr_gbuffer_shader_.setFloat("u_Material.metallicFactor", pbr->metallicFactor());
            pbr_gbuffer_shader_.setFloat("u_Material.roughnessFactor", pbr->roughnessFactor());
            pbr_gbuffer_shader_.setFloat("u_Material.normalScale", pbr->normalScale());
            pbr_gbuffer_shader_.setFloat("u_Material.occlusionStrength", pbr->occlusionStrength());
            pbr_gbuffer_shader_.setVec3("u_Material.emissiveFactor", pbr->emissiveFactor());

            pbr_gbuffer_shader_.setInt("u_Material.hasBaseColorMap", pbr->baseColorMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasMetallicRoughnessMap", pbr->metallicRoughnessMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasNormalMap", pbr->normalMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasOcclusionMap", pbr->occlusionMap() ? 1 : 0);
            pbr_gbuffer_shader_.setInt("u_Material.hasEmissiveMap", pbr->emissiveMap() ? 1 : 0);

            int alpha_mode = 0;
            switch (pbr->alphaMode()) {
                case AlphaMode::Opaque: alpha_mode = 0; break;
                case AlphaMode::Mask:   alpha_mode = 1; break;
                case AlphaMode::Blend:  alpha_mode = 2; break;
            }
            pbr_gbuffer_shader_.setInt("u_Material.alphaMode", alpha_mode);
            pbr_gbuffer_shader_.setFloat("u_Material.alphaCutoff", pbr->alphaCutoff());

            if (pbr->baseColorMap()) {
                pbr->baseColorMap()->bind(0);
                pbr_gbuffer_shader_.setInt("u_BaseColorMap", 0);
            }
            if (pbr->metallicRoughnessMap()) {
                pbr->metallicRoughnessMap()->bind(1);
                pbr_gbuffer_shader_.setInt("u_MetallicRoughnessMap", 1);
            }
            if (pbr->normalMap()) {
                pbr->normalMap()->bind(2);
                pbr_gbuffer_shader_.setInt("u_NormalMap", 2);
            }
            if (pbr->occlusionMap()) {
                pbr->occlusionMap()->bind(3);
                pbr_gbuffer_shader_.setInt("u_OcclusionMap", 3);
            }
            if (pbr->emissiveMap()) {
                pbr->emissiveMap()->bind(4);
                pbr_gbuffer_shader_.setInt("u_EmissiveMap", 4);
            }

            if (pbr->doubleSided()) {
                glDisable(GL_CULL_FACE);
            } else {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }

            item.mesh->draw();
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool GBufferPass::reloadShader() {
        return pbr_gbuffer_shader_.reload();
    }
} // renderer