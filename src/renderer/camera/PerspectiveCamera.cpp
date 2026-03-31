//
// Created by kaede on 2026/3/27.
//

#include "PerspectiveCamera.hpp"

namespace renderer {
    PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, const glm::vec3& front, const glm::vec3& world_up, float fov, float aspect, float z_near, float z_far)
        :Camera(position,front,world_up),fov_(fov), aspect_(aspect), z_near_(z_near), z_far_(z_far)
    {}

    glm::mat4 PerspectiveCamera::getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov_), aspect_, z_near_, z_far_);
    }
}
