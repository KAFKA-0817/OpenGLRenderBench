//
// Created by kaede on 2026/3/27.
//

#include "Renderer.hpp"

#include <algorithm>
#include <unordered_set>

#include "../../core/Log.hpp"
#include "../material/PbrMaterial.hpp"

namespace renderer {
    namespace {
        bool shouldUseInZPre(const RenderItem& item) {
            if (!item.mesh || !item.material) {
                return false;
            }

            if (const auto* pbr_material = dynamic_cast<const PBRMaterial*>(item.material)) {
                if (pbr_material->alphaMode() == AlphaMode::Blend ||
                    pbr_material->alphaMode() == AlphaMode::Mask) {
                    return false;
                }
            }

            return true;
        }
    }

    Renderer::Renderer(int width, int height)
        : gbuffer_pass_(width, height),
          z_pre_pass_(width, height),
          lighting_pass_(width, height),
        bloom_pass_(width, height),
        ssao_pass_(width, height),
        present_pass_(width, height),
        mask_pass_(width, height),
        outline_pass_(width,height),
        shadow_pass_(4096,4096)
    {
    }

    void Renderer::resize(int width, int height) {
        gbuffer_pass_.resize(width, height);
        z_pre_pass_.resize(width, height);
        lighting_pass_.resize(width, height);
        bloom_pass_.resize(width, height);
        ssao_pass_.resize(width, height);
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
            case PreviewMode::SSAO:
                return ssao_enabled_ ? ssao_pass_.colorAttachment() : white_texture_.id();
            case PreviewMode::Mask:
                return mask_pass_.colorAttachment();
            case PreviewMode::Shadow:
                return shadow_pass_.colorAttachment();
            case PreviewMode::BrdfLut:
                return ibl_resources_.brdf_lut.id();
            default:
                return present_pass_.colorAttachment();
        }
    }

    const TextureCube& Renderer::currentSkyboxTexture() const noexcept {
        switch (skybox_source_) {
            case SkyboxSource::Environment:
                return ibl_resources_.environment_map;
            case SkyboxSource::Irradiance:
                return ibl_resources_.irradiance_map;
            case SkyboxSource::Prefilter:
                return ibl_resources_.prefilter_map;
            default:
                return ibl_resources_.environment_map;
        }
    }

    void Renderer::setSkyboxSource(const SkyboxSource source) noexcept {
        skybox_source_ = source;
        setSkyboxLod(skybox_lod_);
    }

    void Renderer::setSkyboxLod(const float lod) noexcept {
        skybox_lod_ = std::clamp(lod, 0.0f, skyboxMaxLod());
    }

    float Renderer::skyboxMaxLod() const noexcept {
        return std::max(0, currentSkyboxTexture().mipCount() - 1);
    }

    void Renderer::renderFrame(const Camera& camera, const RenderContext& render_context) {
        anyEntitySelected_ = false;
        selectedItems_.clear();
        anyEntitySelected_ = isEntitySelected(render_context,selectedItems_);

        std::vector<RenderItem> z_pre_items;
        if (z_pre_enabled_) {
            z_pre_items.reserve(deferred_items_.size() + forward_items_.size());
            for (const auto& item : deferred_items_) {
                if (shouldUseInZPre(item)) {
                    z_pre_items.push_back(item);
                }
            }
            for (const auto& item : forward_items_) {
                if (shouldUseInZPre(item)) {
                    z_pre_items.push_back(item);
                }
            }
        }

        if (render_context.directional_light.valid) {
            std::vector<RenderItem> shadow_items;
            shadow_items.reserve(deferred_items_.size() + forward_items_.size());
            shadow_items.insert(shadow_items.end(), deferred_items_.begin(), deferred_items_.end());
            shadow_items.insert(shadow_items.end(), forward_items_.begin(), forward_items_.end());
            shadow_pass_.setLightCamera(camera, render_context.directional_light.direction);
            shadow_pass_.execute(shadow_items);
        }

        if (z_pre_enabled_ && !z_pre_items.empty()) {
            z_pre_pass_.execute(z_pre_items, camera);
        }

        gbuffer_pass_.execute(
            deferred_items_,
            camera,
            (z_pre_enabled_ && !z_pre_items.empty()) ? &z_pre_pass_.framebuffer() : nullptr
        );

        GLuint ssao_texture = white_texture_.id();
        if (ssao_enabled_) {
            ssao_pass_.execute(
                gbuffer_pass_.gPosition(),
                gbuffer_pass_.gNormal(),
                camera
            );
            ssao_texture = ssao_pass_.colorAttachment();
        }

        lighting_pass_.execute(
            gbuffer_pass_.gPosition(),
            gbuffer_pass_.gNormal(),
            gbuffer_pass_.gAlbedo(),
            gbuffer_pass_.gMaterial(),
            gbuffer_pass_.gEmissive(),
            ssao_texture,
            shadow_pass_.colorAttachment(),
            shadow_pass_.getLightSpaceMatrix(),
            render_context,
            ibl_resources_.irradiance_map,
            ibl_resources_.prefilter_map,
            ibl_resources_.brdf_lut
        );

        FrameBuffer::blitDepth(gbuffer_pass_.framebuffer(), lighting_pass_.framebuffer());

        forward_pass_.execute(forward_items_,
                              camera,
                              render_context,
                              shadow_pass_.colorAttachment(),
                              shadow_pass_.getLightSpaceMatrix(),
                              lighting_pass_.framebuffer(),
                              z_pre_enabled_ && !z_pre_items.empty());

        skybox_pass_.execute(currentSkyboxTexture(), camera, lighting_pass_.framebuffer(), skybox_lod_);

        forward_pass_.executeTransparentPbr(transparent_items_,
                                            camera,
                                            render_context,
                                            lighting_pass_.framebuffer(),
                                            ibl_resources_.irradiance_map,
                                            ibl_resources_.prefilter_map,
                                            ibl_resources_.brdf_lut);

        GLuint scene_hdr_texture = lighting_pass_.colorAttachment();
        if (bloom_enabled_) {
            bloom_pass_.execute(scene_hdr_texture);
            scene_hdr_texture = bloom_pass_.colorAttachment();
        }
        present_pass_.present(scene_hdr_texture,render_context.exposure);

        if (anyEntitySelected_) {
            FrameBuffer::blitDepth(lighting_pass_.framebuffer(),mask_pass_.framebuffer());
            mask_pass_.execute(selectedItems_,camera);
            outline_pass_.execute(mask_pass_.colorAttachment(),present_pass_.colorAttachment());
        }
    }

    void Renderer::reloadBuiltinShaders() {
        gbuffer_pass_.reloadShader();
        z_pre_pass_.reloadShader();
        lighting_pass_.reloadShader();
        bloom_pass_.reloadShader();
        ssao_pass_.reloadShader();
        forward_pass_.reloadShader();
        present_pass_.reloadShader();
        mask_pass_.reloadShader();
        outline_pass_.reloadShader();
        shadow_pass_.reloadShader();
        skybox_pass_.reloadShader();
    }

    void Renderer::reloadShaders(const std::vector<std::filesystem::path>& dirty_files) {
        std::unordered_set<ShaderType> shaders_to_reload;
        auto isShaderFile = [](const std::filesystem::path& path) {
            const auto extension = path.extension().string();
            return extension == ".vs" ||
                   extension == ".fs" ||
                   extension == ".vert" ||
                   extension == ".frag" ||
                   extension == ".glsl";
        };
        for (const auto& file:dirty_files) {
            if (!isShaderFile(file)) continue;

            const auto filename = file.stem().string();
            auto it = MAP_FILENAME_SHADER.find(filename);
            if (it==MAP_FILENAME_SHADER.end()) continue;
            shaders_to_reload.insert(it->second);
        }

        for (const auto& shader:shaders_to_reload) {
            switch (shader) {
                case ShaderType::Bloom:
                    bloom_pass_.reloadShader();
                    break;
                case ShaderType::Forward:
                    forward_pass_.reloadShader();
                    break;
                case ShaderType::GBuffer:
                    gbuffer_pass_.reloadShader();
                    break;
                case ShaderType::Lighting:
                    lighting_pass_.reloadShader();
                    break;
                case ShaderType::Mask:
                    mask_pass_.reloadShader();
                    break;
                case ShaderType::Outline:
                    outline_pass_.reloadShader();
                    break;
                case ShaderType::Present:
                    present_pass_.reloadShader();
                    break;
                case ShaderType::Shadow:
                    shadow_pass_.reloadShader();
                    break;
                case ShaderType::Skybox:
                    skybox_pass_.reloadShader();
                    break;
                case ShaderType::SSAO:
                    ssao_pass_.reloadShader();
                    break;
                case ShaderType::ZPre:
                    z_pre_pass_.reloadShader();
                    break;
            }
        }
    }

    bool Renderer::loadEnvironmentMap(const std::filesystem::path& path) {
        try {
            ibl_resources_.environment_map = TextureCube::createFromKtx2(path);
            core::Log::getInstance().write("Renderer", "Loaded environment map: " + path.string());
            return true;
        } catch (const std::exception& e) {
            core::Log::getInstance().write("Renderer", std::string("Failed to load environment map: ") + e.what());
            return false;
        }
    }

    bool Renderer::loadIrradianceMap(const std::filesystem::path& path) {
        try {
            ibl_resources_.irradiance_map = TextureCube::createFromKtx2(path);
            core::Log::getInstance().write("Renderer", "Loaded irradiance map: " + path.string());
            return true;
        } catch (const std::exception& e) {
            core::Log::getInstance().write("Renderer", std::string("Failed to load irradiance map: ") + e.what());
            return false;
        }
    }

    bool Renderer::loadPrefilterMap(const std::filesystem::path& path) {
        try {
            ibl_resources_.prefilter_map = TextureCube::createFromKtx2(path);
            setSkyboxLod(skybox_lod_);
            core::Log::getInstance().write("Renderer", "Loaded prefilter map: " + path.string());
            return true;
        } catch (const std::exception& e) {
            core::Log::getInstance().write("Renderer", std::string("Failed to load prefilter map: ") + e.what());
            return false;
        }
    }

    bool Renderer::loadBrdfLut(const std::filesystem::path& path) {
        try {
            ibl_resources_.brdf_lut = Texture2D::createFromKtx2(path);
            core::Log::getInstance().write("Renderer", "Loaded BRDF LUT: " + path.string());
            return true;
        } catch (const std::exception& e) {
            core::Log::getInstance().write("Renderer", std::string("Failed to load BRDF LUT: ") + e.what());
            return false;
        }
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
