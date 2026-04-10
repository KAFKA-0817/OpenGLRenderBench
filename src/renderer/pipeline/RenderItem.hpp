//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_RENDERITEM_HPP
#define PBRRENDERER_RENDERITEM_HPP
#include "../../editor/scene/Entity.hpp"
#include "../asset/Model.hpp"
#include "../material/Material.hpp"
#include "glm/ext/matrix_double4x4.hpp"

namespace renderer {

    struct RenderItem {
        editor::Entity id = editor::kInvalidEntity;
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        glm::mat4 model_matrix{1.0f};
    };

}

#endif //PBRRENDERER_RENDERITEM_HPP