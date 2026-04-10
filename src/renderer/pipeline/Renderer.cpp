//
// Created by kaede on 2026/3/27.
//

#include "Renderer.hpp"

#include "../../core/Log.hpp"
#include "../asset/PrimitiveFactory.hpp"
#include "../material/PbrMaterial.hpp"

namespace renderer {
    Renderer::Renderer(int width, int height)
        : gbuffer_pass_(width, height),
          lighting_pass_(width, height),
        present_pass_(width, height),
        mask_pass_(width, height),
        outline_pass_(width,height)
    {
    }

    void Renderer::resize(int width, int height) {
        gbuffer_pass_.resize(width, height);
        lighting_pass_.resize(width, height);
        present_pass_.resize(width, height);
        mask_pass_.resize(width, height);
        outline_pass_.resize(width,height);
    }

    void Renderer::submit( editor::Entity id,
                        const Mesh& mesh,
                      const Material& material,
                      const glm::mat4& model_matrix) {
        RenderItem item{
            id,
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
                return anyEntitySelected_? outline_pass_.colorAttachment() : present_pass_.colorAttachment();
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
            case PreviewMode::Mask:
                return mask_pass_.colorAttachment();
            default:
                return present_pass_.colorAttachment();
        }
    }

    void Renderer::renderFrame(const Camera& camera, const RenderContext& render_context) {
        anyEntitySelected_ = false;
        selectedItems_.clear();
        anyEntitySelected_ = isEntitySelected(render_context,selectedItems_);

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

        present_pass_.present(lighting_pass_.colorAttachment(),render_context.exposure);

        if (anyEntitySelected_) {
            FrameBuffer::blitDepth(lighting_pass_.framebuffer(),mask_pass_.framebuffer());
            mask_pass_.execute(selectedItems_,camera);
            outline_pass_.execute(mask_pass_.colorAttachment(),present_pass_.colorAttachment());
        }
    }

    void Renderer::reloadBuiltinShaders() {
        gbuffer_pass_.reloadShader();
        lighting_pass_.reloadShader();
        forward_pass_.reloadShader();
        present_pass_.reloadShader();
        mask_pass_.reloadShader();
        outline_pass_.reloadShader();
    }

    bool Renderer::isEntitySelected(const RenderContext& render_context, std::vector<RenderItem>& selectedItems) const {
        if (render_context.selected == editor::kInvalidEntity) return false;

        bool anySelected = false;
        for (auto&& item:deferred_items_) {
            if (item.id == render_context.selected) {
                selectedItems.push_back(item);
                anySelected = true;
            }
        }
        for (auto&& item:forward_items_) {
            if (item.id == render_context.selected) {
                selectedItems.push_back(item);
                anySelected = true;
            }
        }
        for (auto&& item:transparent_items_) {
            if (item.id == render_context.selected) {
                selectedItems.push_back(item);
                anySelected = true;
            }
        }

        return anySelected;
    }
}
