//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_RENDER_HPP
#define PBRRENDERER_RENDER_HPP
#include "BloomPass.hpp"
#include "ForwardPass.hpp"
#include "GBufferPass.hpp"
#include "PresentPass.hpp"
#include "../../app/frame/RenderContextFrame.hpp"
#include "../../core/opengl.hpp"
#include "../../core/noncopyable.hpp"
#include "../asset/Texture2D.hpp"
#include "LightingPass.hpp"
#include "MaskPass.hpp"
#include "OutlinePass.hpp"
#include "SSAOPass.hpp"
#include "ShadowPass.hpp"

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

        void submit(editor::Entity id,
                const Mesh& mesh,
                const Material& material,
                const glm::mat4& model_matrix);
        void clearSubmissions();
        void renderFrame(const Camera& camera, const RenderContext& render_context);
        void reloadBuiltinShaders();
        GLuint outputTexture() const noexcept { return currentPreviewTexture(); }

    private:
        GLuint currentPreviewTexture() const noexcept;
        bool isEntitySelected(const RenderContext& render_context, std::vector<RenderItem>& selectedItems) const;

    private:
        GBufferPass gbuffer_pass_;
        LightingPass lighting_pass_;
        ForwardPass forward_pass_;
        BloomPass bloom_pass_;
        SSAOPass ssao_pass_;
        PresentPass present_pass_;
        MaskPass mask_pass_;
        OutlinePass outline_pass_;
        ShadowPass shadow_pass_;

        bool anyEntitySelected_ = false;
        std::vector<RenderItem> selectedItems_;
        std::vector<RenderItem> deferred_items_;
        std::vector<RenderItem> forward_items_;
        std::vector<RenderItem> transparent_items_;
        Texture2D white_texture_ = Texture2D::createWhite1x1();
        bool ssao_enabled_ = true;
        PreviewMode preview_mode_ = PreviewMode::FinalScene;
        float clear_r_ = 0.1f;
        float clear_g_ = 0.1f;
        float clear_b_ = 0.1f;
        float clear_a_ = 1.0f;
    };
}




#endif //PBRRENDERER_RENDER_HPP
