//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_RENDER_HPP
#define PBRRENDERER_RENDER_HPP
#include <filesystem>

#include "BloomPass.hpp"
#include "ForwardPass.hpp"
#include "GBufferPass.hpp"
#include "PresentPass.hpp"
#include "ZPrePass.hpp"
#include "../../app/frame/RenderContextFrame.hpp"
#include "../../core/opengl.hpp"
#include "../../core/noncopyable.hpp"
#include "../asset/Texture2D.hpp"
#include "../asset/TextureCube.hpp"
#include "LightingPass.hpp"
#include "MaskPass.hpp"
#include "OutlinePass.hpp"
#include "SSAOPass.hpp"
#include "ShadowPass.hpp"
#include "SkyboxPass.hpp"

namespace renderer {

    enum class PreviewMode {
        FinalScene = 0,
        GPosition = 1,
        GNormal = 2,
        GAlbedo = 3,
        GMaterial = 4,
        GEmissive = 5,
        SSAO = 6,
        Mask = 7,
        Shadow = 8,
        BrdfLut = 9,
    };

    enum class SkyboxSource {
        Environment = 0,
        Irradiance = 1,
        Prefilter = 2,
    };

    enum class ShaderType {
        Bloom,
        Forward,
        GBuffer,
        Lighting,
        Mask,
        Outline,
        Present,
        Shadow,
        Skybox,
        SSAO,
        ZPre
    };

    class Renderer : public core::NonCopyable {
    public:
        Renderer(int width, int height);
        ~Renderer() = default;
        Renderer(Renderer&& other) = default;
        Renderer& operator=(Renderer&& other) = default;

        void resize(int width, int height);
        int width() const noexcept { return gbuffer_pass_.width(); }
        int height() const noexcept { return gbuffer_pass_.height(); }
        void setClearColor(float r, float g, float b, float a) noexcept { clear_r_ = r; clear_g_ = g; clear_b_ = b; clear_a_ = a; }
        void setPreviewMode(PreviewMode mode) noexcept { preview_mode_ = mode; }
        PreviewMode previewMode() const noexcept { return preview_mode_; }
        void setSSAOEnabled(bool enabled) noexcept { ssao_enabled_ = enabled; }
        bool ssaoEnabled() const noexcept { return ssao_enabled_; }
        void setBloomEnabled(bool enabled) noexcept { bloom_enabled_ = enabled; }
        bool bloomEnabled() const noexcept { return bloom_enabled_; }
        void setZPreEnabled(bool enabled) noexcept { z_pre_enabled_ = enabled; }
        bool zPreEnabled() const noexcept { return z_pre_enabled_; }
        void setSkyboxSource(SkyboxSource source) noexcept;
        SkyboxSource skyboxSource() const noexcept { return skybox_source_; }
        void setSkyboxLod(float lod) noexcept;
        float skyboxLod() const noexcept { return skybox_lod_; }
        float skyboxMaxLod() const noexcept;

        void submit(editor::Entity id,
                const Mesh& mesh,
                const Material& material,
                const glm::mat4& model_matrix);
        void clearSubmissions();
        void renderFrame(const Camera& camera, const RenderContext& render_context);
        void reloadBuiltinShaders();
        void reloadShaders(const std::vector<std::filesystem::path>&);
        bool loadBrdfLut(const std::filesystem::path& path);
        bool loadEnvironmentMap(const std::filesystem::path& path);
        bool loadIrradianceMap(const std::filesystem::path& path);
        bool loadPrefilterMap(const std::filesystem::path& path);
        GLuint outputTexture() const noexcept { return currentPreviewTexture(); }

    private:
        struct IblResources {
            Texture2D brdf_lut;
            TextureCube environment_map;
            TextureCube irradiance_map;
            TextureCube prefilter_map;
        };

        GLuint currentPreviewTexture() const noexcept;
        const TextureCube& currentSkyboxTexture() const noexcept;
        bool isEntitySelected(const RenderContext& render_context, std::vector<RenderItem>& selectedItems) const;

    private:
        GBufferPass gbuffer_pass_;
        ZPrePass z_pre_pass_;
        LightingPass lighting_pass_;
        ForwardPass forward_pass_;
        BloomPass bloom_pass_;
        SSAOPass ssao_pass_;
        PresentPass present_pass_;
        MaskPass mask_pass_;
        OutlinePass outline_pass_;
        ShadowPass shadow_pass_;
        SkyboxPass skybox_pass_;

        bool anyEntitySelected_ = false;
        std::vector<RenderItem> selectedItems_;
        std::vector<RenderItem> deferred_items_;
        std::vector<RenderItem> forward_items_;
        std::vector<RenderItem> transparent_items_;
        IblResources ibl_resources_;
        Texture2D white_texture_ = Texture2D::createWhite1x1();
        bool ssao_enabled_ = true;
        bool bloom_enabled_ = true;
        bool z_pre_enabled_ = false;
        SkyboxSource skybox_source_ = SkyboxSource::Environment;
        PreviewMode preview_mode_ = PreviewMode::FinalScene;
        float skybox_lod_ = 0.0f;
        float clear_r_ = 0.1f;
        float clear_g_ = 0.1f;
        float clear_b_ = 0.1f;
        float clear_a_ = 1.0f;

    private:
        inline static const std::unordered_map<std::string,ShaderType> MAP_FILENAME_SHADER = {
            {"bloom_extract",ShaderType::Bloom},
            {"bloom_blur",ShaderType::Bloom},
            {"bloom_composite",ShaderType::Bloom},
            {"forward_pbr",ShaderType::Forward},
            {"gbuffer_pbr",ShaderType::GBuffer},
            {"lighting_pass",ShaderType::Lighting},
            {"mask",ShaderType::Mask},
            {"outline",ShaderType::Outline},
            {"screen",ShaderType::Present},
            {"shadow_pass",ShaderType::Shadow},
            {"skybox",ShaderType::Skybox},
            {"ssao",ShaderType::SSAO},
            {"ssao_blur",ShaderType::SSAO},
            {"z_pre",ShaderType::ZPre}
        };
    };
}




#endif //PBRRENDERER_RENDER_HPP
