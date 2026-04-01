//
// Created by kaede on 2026/3/27.
//

#include "Model.hpp"

namespace renderer {
    void Model::add_mesh(Mesh mesh, int material_index) {
        meshes_.push_back({std::move(mesh), material_index});
    }

    void Model::draw() const {
        for (const auto& mesh : meshes_) {
            mesh.mesh.draw();
        }
    }
}
