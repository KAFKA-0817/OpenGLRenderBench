//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_IMPORTEDMODELDATA_HPP
#define PBRRENDERER_IMPORTEDMODELDATA_HPP
#include <filesystem>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include "../Vertex.hpp"

namespace renderer {

    struct ImageData {
        std::string name;
        int width = 0;
        int height = 0;
        int channels = 0;
        bool srgb = false;
        std::vector<unsigned char> pixels;
    };

    struct PBRMaterialData {
        std::string name;

        glm::vec4 base_color_factor{1.0f, 1.0f, 1.0f, 1.0f};
        float metallic_factor = 1.0f;
        float roughness_factor = 1.0f;
        float normal_scale = 1.0f;
        float occlusion_strength = 1.0f;
        glm::vec3 emissive_factor{0.0f, 0.0f, 0.0f};

        int base_color_image = -1;         //albedo
        int metallic_roughness_image = -1; //metallic + roughness
        int normal_image = -1;             //normal
        int occlusion_image = -1;          //ao
        int emissive_image = -1;           //emissive

        AlphaMode alpha_mode = AlphaMode::Opaque;
        float alpha_cutoff = 0.5f;
        bool double_sided = false;
    };

    struct SubmeshData {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        int material_index = -1;
    };

    struct ImportedModelData {
        std::string name;
        std::filesystem::path path;

        std::vector<ImageData> images;
        std::vector<PBRMaterialData> materials;
        std::vector<SubmeshData> submeshes;
    };
}

#endif //PBRRENDERER_IMPORTEDMODELDATA_HPP