//
// Created by kaede on 2026/3/27.
//

#include "Model.hpp"

namespace renderer {
    void Model::add_mesh(Mesh mesh, int material_index) {
        meshes_.push_back({std::move(mesh), material_index});
    }

    void Model::add_material(std::unique_ptr<Material> material) {
        materials_.push_back(std::move(material));
    }

    void Model::add_texture(Texture2D texture) {
        textures_.push_back(std::move(texture));
    }
}
