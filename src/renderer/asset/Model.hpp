//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_MODEL_HPP
#define PBRRENDERER_MODEL_HPP
#include <vector>

#include "Mesh.hpp"
#include "../../core/noncopyable.hpp"

namespace renderer {
    class Model :public core::NonCopyable {
    public:
        Model() = default;
        ~Model() = default;
        Model(Model&& other) noexcept = default;
        Model& operator=(Model&& other) noexcept = default;

        void draw() const;
        void add_mesh(Mesh mesh);
        bool empty() const { return meshes_.empty(); }
    private:
        std::vector<Mesh> meshes_;
    };
}



#endif //PBRRENDERER_MODEL_HPP