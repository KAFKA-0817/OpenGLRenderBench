//
// Created by kaede on 2026/3/31.
//

#include "RuntimeAssetBuilder.hpp"

#include "../Texture2D.hpp"
#include "../../material/PbrMaterial.hpp"

namespace renderer {

    namespace {

        void buildTextures(const ImportedModelData& data, Model& asset) {
            asset.textures().reserve(data.images.size());

            for (const auto& img : data.images) {
                if (img.pixels.empty() || img.width <= 0 || img.height <= 0 || img.channels <= 0) {
                    asset.textures().emplace_back();
                    continue;
                }

                asset.textures().emplace_back(
                    Texture2D::createFromPixels(
                        img.pixels.data(),
                        img.width,
                        img.height,
                        img.channels,
                        img.srgb
                    )
                );
            }
        }

        void buildMaterial(const ImportedModelData& data, Model& asset) {
            asset.materials().reserve(data.materials.size());
            for (const auto& mat : data.materials) {
                auto material = std::make_unique<PBRMaterial>();

                material->setBaseColorFactor(mat.base_color_factor);
                material->setMetallicFactor(mat.metallic_factor);
                material->setRoughnessFactor(mat.roughness_factor);
                material->setNormalScale(mat.normal_scale);
                material->setOcclusionStrength(mat.occlusion_strength);
                material->setEmissiveFactor(mat.emissive_factor);
                material->setAlphaMode(mat.alpha_mode);
                material->setAlphaCutoff(mat.alpha_cutoff);
                material->setDoubleSided(mat.double_sided);

                if (mat.base_color_image >= 0) {
                    material->setBaseColorMap(&asset.textures()[mat.base_color_image]);
                }

                if (mat.metallic_roughness_image >= 0) {
                    material->setMetallicRoughnessMap(&asset.textures()[mat.metallic_roughness_image]);
                }

                if (mat.normal_image >= 0) {
                    material->setNormalMap(&asset.textures()[mat.normal_image]);
                }

                if (mat.occlusion_image >= 0) {
                    material->setOcclusionMap(&asset.textures()[mat.occlusion_image]);
                }

                if (mat.emissive_image >= 0) {
                    material->setEmissiveMap(&asset.textures()[mat.emissive_image]);
                }

                asset.materials().emplace_back(std::move(material));
            }

        }

    }

    Model RuntimeAssetBuilder::buildFromImported(const ImportedModelData& data) {
        Model asset;
        buildTextures(data,asset);
        buildMaterial(data,asset);

        for (const auto& submesh_data : data.submeshes) {
            Mesh mesh(submesh_data.vertices, submesh_data.indices);
            asset.add_mesh(
                std::move(mesh),
                submesh_data.material_index
            );
        }

        return asset;
    }
} // renderer