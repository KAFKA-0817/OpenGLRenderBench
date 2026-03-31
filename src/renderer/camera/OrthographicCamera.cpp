//
// Created by kaede on 2026/3/27.
//

#include "OrthographicCamera.hpp"

namespace renderer {
    OrthographicCamera::OrthographicCamera(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &world_up, float left, float right, float bottom, float top, float z_near, float z_far)
        :Camera(position,front,world_up),left_(left),right_(right),bottom_(bottom),top_(top),z_near_(z_near),z_far_(z_far)
    {

    }

    glm::mat4 OrthographicCamera::getProjectionMatrix() const {
        return glm::ortho(left_, right_, bottom_, top_, z_near_, z_far_);
    }
}
