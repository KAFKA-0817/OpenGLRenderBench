//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_RUNTIMEMODELASSET_HPP
#define PBRRENDERER_RUNTIMEMODELASSET_HPP
#include <memory>
#include <vector>
#include "../Model.hpp"
#include "../Texture2D.hpp"
#include "../../material/Material.hpp"

namespace renderer {
    struct RuntimeModelAsset {
        Model model;
        std::vector<Texture2D> textures;
        std::vector<std::unique_ptr<Material>> materials;
    };

}

#endif //PBRRENDERER_RUNTIMEMODELASSET_HPP