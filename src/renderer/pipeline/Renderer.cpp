//
// Created by kaede on 2026/3/27.
//

#include "Renderer.hpp"

#include "../../core/path.hpp"
#include "../asset/PrimitiveFactory.hpp"
#include "../material/PbrMaterial.hpp"

namespace renderer {
    Renderer::Renderer(int width, int height)
        :gbuffer_pass_(width, height),
        lighting_pass_(width, height)
    {

    }

    void Renderer::resize(int width, int height) {
        gbuffer_pass_.resize(width, height);
        lighting_pass_.resize(width, height);
    }

    void Renderer::submit(const Mesh& mesh,
                      const Material& material,
                      const glm::mat4& model_matrix) {
        RenderItem item{
            &mesh,
            &material,
            model_matrix
        };

        if (const auto* pbr_material = dynamic_cast<const PBRMaterial*>(&material);
            pbr_material && pbr_material->alphaMode() == AlphaMode::Blend) {
            transparent_items_.push_back(item);
            return;
        }

        if (material.renderPath() == RenderPath::Deferred) {
            deferred_items_.push_back(item);
        } else {
            forward_items_.push_back(item);
        }
    }

    void Renderer::clearSubmissions() {
        deferred_items_.clear();
        forward_items_.clear();
        transparent_items_.clear();
    }

    GLuint Renderer::currentPreviewTexture() const noexcept {
        switch (preview_mode_) {
            case PreviewMode::FinalScene:
                return lighting_pass_.colorAttachment();
            case PreviewMode::GPosition:
                return gbuffer_pass_.gPosition();
            case PreviewMode::GNormal:
                return gbuffer_pass_.gNormal();
            case PreviewMode::GAlbedo:
                return gbuffer_pass_.gAlbedo();
            case PreviewMode::GMaterial:
                return gbuffer_pass_.gMaterial();
            case PreviewMode::GEmissive:
                return gbuffer_pass_.gEmissive();
            default:
                return lighting_pass_.colorAttachment();
        }
    }

    void Renderer::renderFrame(const Camera& camera, const RenderContext& render_context) {
        gbuffer_pass_.execute(deferred_items_, camera);

        lighting_pass_.execute(
            gbuffer_pass_.gPosition(),
            gbuffer_pass_.gNormal(),
            gbuffer_pass_.gAlbedo(),
            gbuffer_pass_.gMaterial(),
            gbuffer_pass_.gEmissive(),
            render_context
        );

        FrameBuffer::blitDepth(gbuffer_pass_.framebuffer(), lighting_pass_.framebuffer());

        forward_pass_.execute(forward_items_,
                              camera,
                              render_context,
                              lighting_pass_.framebuffer());

        forward_pass_.executeTransparentPbr(transparent_items_,
                                            camera,
                                            render_context,
                                            lighting_pass_.framebuffer());

        if (present_to_screen_enabled_) {
            present_pass_.present(currentPreviewTexture(),
                              width(),
                              height());
        }

    }

    void Renderer::reloadBuiltinShaders() {
        gbuffer_pass_.reloadShader();
        lighting_pass_.reloadShader();
        forward_pass_.reloadShader();
        present_pass_.reloadShader();
    }
}
