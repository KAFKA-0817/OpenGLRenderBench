//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_MODEL_HPP
#define PBRRENDERER_MODEL_HPP
#include <memory>
#include <vector>
#include "Mesh.hpp"
#include "Texture2D.hpp"
#include "../../core/noncopyable.hpp"
#include "../material/Material.hpp"

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

        void add_mesh(Mesh mesh, int material_index = -1);
        void add_material(std::unique_ptr<Material> material);
        void add_texture(Texture2D texture);
        bool empty() const { return meshes_.empty(); }
        const std::vector<SubMesh>& meshes() const noexcept { return meshes_; }
        const std::vector<std::unique_ptr<Material>>& materials() const noexcept { return materials_; }
        const std::vector<Texture2D>& textures() const noexcept { return textures_; }
        std::vector<SubMesh>& meshes() noexcept { return meshes_; }
        std::vector<std::unique_ptr<Material>>& materials() noexcept { return materials_; }
        std::vector<Texture2D>& textures() noexcept { return textures_; }

    private:
        std::vector<SubMesh> meshes_;
        std::vector<std::unique_ptr<Material>> materials_;
        std::vector<Texture2D> textures_;
    };
}



#endif //PBRRENDERER_MODEL_HPP