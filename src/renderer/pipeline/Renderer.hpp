//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_RENDER_HPP
#define PBRRENDERER_RENDER_HPP
#include "ForwardPass.hpp"
#include "FragPosPass.hpp"
#include "GBufferPass.hpp"
#include "NormalPass.hpp"
#include "PresentPass.hpp"
#include "../../core/opengl.hpp"
#include "../asset/Model.hpp"
#include "../core/Shader.hpp"
#include "../../core/noncopyable.hpp"
#include "../camera/Camera.hpp"
#include "../material/Material.hpp"
#include "LightingPass.hpp"

namespace renderer {

    enum class PreviewMode {
        FinalScene = 0,
        GPosition = 1,
        GNormal = 2,
        GAlbedo = 3,
        GMaterial = 4,
        GEmissive = 5
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
        RenderContext& renderContext() noexcept { return render_context_; }
        const RenderContext& renderContext() const noexcept { return render_context_; }

        void submit(const Mesh& mesh,
                const Material& material,
                const glm::mat4& model_matrix);
        void clearSubmissions();
        void renderFrame(const Camera& camera);
        void reloadBuiltinShaders();

    private:
        GLuint currentPreviewTexture() const noexcept;

    private:
        GBufferPass gbuffer_pass_;
        LightingPass lighting_pass_;
        ForwardPass forward_pass_;
        PresentPass present_pass_;

        RenderContext render_context_{};
        std::vector<RenderItem> deferred_items_;
        std::vector<RenderItem> forward_items_;
        PreviewMode preview_mode_ = PreviewMode::FinalScene;
        float clear_r_ = 0.1f;
        float clear_g_ = 0.1f;
        float clear_b_ = 0.1f;
        float clear_a_ = 1.0f;
    };
}




#endif //PBRRENDERER_RENDER_HPP
