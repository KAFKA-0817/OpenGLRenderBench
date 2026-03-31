//
// Created by kaede on 2026/3/27.
//

#include "Model.hpp"

namespace renderer {
    void Model::add_mesh(Mesh mesh) {
        meshes_.push_back(std::move(mesh));
    }

    void Model::draw() const {
        for (const auto& mesh : meshes_) {
            mesh.draw();
        }
    }
}
