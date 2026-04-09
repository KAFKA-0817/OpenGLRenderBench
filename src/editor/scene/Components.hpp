//
// Created by kaede on 2026/4/3.
//

#ifndef PBRRENDERER_COMPONENTS_HPP
#define PBRRENDERER_COMPONENTS_HPP
#include <string>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "../../renderer/asset/Model.hpp"

namespace editor {

    struct NameComponent {
        std::string name;
    };

    struct TransformComponent {
        glm::vec3 position = {0.0f,0.0f,0.0f};
        glm::quat rotation = {1.0f,0.0f,0.0f,0.0f};
        glm::vec3 scale = {1.0f,1.0f,1.0f};

        static glm::quat quatFromEulerDegrees(const glm::vec3& euler_degrees) {
            return glm::normalize(glm::quat_cast(glm::eulerAngleXYZ(
                glm::radians(euler_degrees.x),
                glm::radians(euler_degrees.y),
                glm::radians(euler_degrees.z)
            )));
        }

        static TransformComponent fromEulerDegrees(
            const glm::vec3& position,
            const glm::vec3& rotation_degrees,
            const glm::vec3& scale = glm::vec3(1.0f)
        ) {
            TransformComponent transform{};
            transform.position = position;
            transform.rotation = quatFromEulerDegrees(rotation_degrees);
            transform.scale = scale;
            return transform;
        }

        void setEulerDegrees(const glm::vec3& euler_degrees) {
            rotation = quatFromEulerDegrees(euler_degrees);
        }

        glm::vec3 eulerDegrees() const {
            float angle_x = 0.0f;
            float angle_y = 0.0f;
            float angle_z = 0.0f;
            glm::extractEulerAngleXYZ(glm::mat4_cast(glm::normalize(rotation)), angle_x, angle_y, angle_z);
            return glm::degrees(glm::vec3(angle_x, angle_y, angle_z));
        }

        glm::vec3 forwardDirection() const {
            return glm::normalize(glm::normalize(rotation) * glm::vec3(0.0f, 0.0f, -1.0f));
        }

        glm::mat4 modelMatrix() const {
            glm::mat4 model(1.0f);
            model = glm::translate(model,position);
            model *= glm::mat4_cast(glm::normalize(rotation));
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
