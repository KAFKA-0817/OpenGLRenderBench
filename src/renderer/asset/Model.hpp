//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_MODEL_HPP
#define PBRRENDERER_MODEL_HPP
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>

#include "Mesh.hpp"
#include "../../core/noncopyable.hpp"

namespace renderer {
    struct SubMesh {
        Mesh mesh;
        int material_index = -1;
    };

    class Model :public core::NonCopyable {
    public:
        Model() = default;
        ~Model() = default;
        Model(Model&& other) noexcept = default;
        Model& operator=(Model&& other) noexcept = default;

        void draw() const;
        void add_mesh(Mesh mesh, int material_index = -1);
        bool empty() const { return meshes_.empty(); }
        const std::vector<SubMesh>& meshes() const noexcept { return meshes_; }
    private:
        std::vector<SubMesh> meshes_;
    };
}



#endif //PBRRENDERER_MODEL_HPP