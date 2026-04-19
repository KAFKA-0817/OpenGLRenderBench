//
// Created by kaede on 2026/3/30.
//

#include "LightingPass.hpp"

#include <algorithm>
#include <string>

#include "../../core/Log.hpp"
#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"


namespace renderer {
    LightingPass::LightingPass(int width, int height)
    : framebuffer_(width, height, 1, GL_RGBA16F),
      screen_quad_(PrimitiveFactory::createQuad()),
      shader_(
          core::ProjectPaths::shader("lighting_pass.vs"),
          core::ProjectPaths::shader("lighting_pass.fs"))
    {}

    void LightingPass::resize(int width, int height) {
        framebuffer_.resize(width, height);
    }

    void LightingPass::execute(GLuint g_position,
                           GLuint g_normal,
                           GLuint g_albedo,
                           GLuint g_material,
                           GLuint g_emissive,
                           GLuint ssao_map,
                           GLuint shadow_map,
                           const glm::mat4& lightSpaceMatrix,
                           const RenderContext& context,
                           const TextureCube& irradiance_map,
                           const TextureCube& prefilter_map,
                           const Texture2D& brdf_lut) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Lighting Pass");

        framebuffer_.bind();
        glViewport(0, 0, framebuffer_.width(), framebuffer_.height());
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader_.use();

        shader_.setVec3("u_ViewPos", context.camera_position);
        shader_.setInt("u_HasDirectionalLight", context.directional_light.valid ? 1 : 0);
        shader_.setInt("u_PointLightCount", static_cast<int>(context.point_light_count));
        shader_.setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

        if (context.directional_light.valid) {
            shader_.setVec3("u_DirectionalLight.direction", context.directional_light.direction);
            shader_.setVec3("u_DirectionalLight.color", context.directional_light.color);
            shader_.setFloat("u_DirectionalLight.intensity", context.directional_light.intensity);
        }

        for (std::uint32_t i = 0; i < context.point_light_count; ++i) {
            const auto& light = context.point_lights[i];
            const std::string prefix = "u_PointLights[" + std::to_string(i) + "]";
            shader_.setVec3(prefix + ".position", light.position);
            shader_.setFloat(prefix + ".range", light.range);
            shader_.setVec3(prefix + ".color", light.color);
            shader_.setFloat(prefix + ".intensity", light.intensity);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_position);
        shader_.setInt("u_GPosition", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        shader_.setInt("u_GNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albedo);
        shader_.setInt("u_GAlbedo", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, g_material);
        shader_.setInt("u_GMaterial", 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, g_emissive);
        shader_.setInt("u_GEmissive", 4);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ssao_map);
        shader_.setInt("u_SSAOMap", 5);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, shadow_map);
        shader_.setInt("u_ShadowMap", 6);

        irradiance_map.bind(7);
        shader_.setInt("u_IrradianceMap", 7);

        prefilter_map.bind(8);
        shader_.setInt("u_PrefilterMap", 8);
        shader_.setFloat("u_PrefilterMaxLod", static_cast<float>(std::max(0, prefilter_map.mipCount() - 1)));

        brdf_lut.bind(9);
        shader_.setInt("u_BrdfLut", 9);

        screen_quad_.draw();

        framebuffer_.unbind();
        glPopDebugGroup();
    }

    bool LightingPass::reloadShader() {
        core::Log::getInstance().write("Shader","Lighting-pass shader reloaded");
        return shader_.reload();
    }
} // renderer
