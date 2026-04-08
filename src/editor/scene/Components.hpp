//
// Created by kaede on 2026/4/3.
//

#ifndef PBRRENDERER_COMPONENTS_HPP
#define PBRRENDERER_COMPONENTS_HPP
#include <string>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../renderer/asset/Model.hpp"

namespace editor {

    struct NameComponent {
        std::string name;
    };

    struct TransformComponent {
        glm::vec3 position = {0.0f,0.0f,0.0f};
        glm::vec3 rotation = {0.0f,0.0f,0.0f};
        glm::vec3 scale = {1.0f,1.0f,1.0f};

        glm::mat4 modelMatrix() const {
            glm::mat4 model(1.0f);
            model = glm::translate(model,position);
            model = glm::rotate(model,glm::radians(rotation.x),glm::vec3(1.0f,0.0f,0.0f));
            model = glm::rotate(model,glm::radians(rotation.y),glm::vec3(0.0f,1.0f,0.0f));
            model = glm::rotate(model,glm::radians(rotation.z),glm::vec3(0.0f,0.0f,1.0f));
            model = glm::scale(model,scale);
            return model;
        }
    };

    struct MeshRendererComponent {
        renderer::Model* model = nullptr;
        bool visible = true;
    };

    struct DirectionalLightComponent {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        float intensity = 1.0f;
        bool enabled = true;
    };

    struct PointLightComponent {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        float intensity = 1.0f;
        float range = 10.0f;
        bool enabled = true;
    };
}

#endif //PBRRENDERER_COMPONENTS_HPP
