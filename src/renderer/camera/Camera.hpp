//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_CAMERA_HPP
#define PBRRENDERER_CAMERA_HPP
#include "../../core/noncopyable.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer {
    class Camera :public core::NonCopyable{
    public:
        Camera() = default;
        Camera(const glm::vec3& position, const glm::vec3& front, const glm::vec3& world_up) {
            position_ = position;
            front_ = glm::normalize(front);
            world_up_ = glm::normalize(world_up);
        }
        virtual ~Camera() = default;
        Camera(Camera&& other) = default;
        Camera& operator=(Camera&& other) = default;

        glm::mat4 getViewMatrix() const {
            return glm::lookAt(position_, position_ + front_, world_up_);
        }
        virtual glm::mat4 getProjectionMatrix() const = 0;

        void setPosition(const glm::vec3& position) noexcept { position_ = position; }
        void setFront(const glm::vec3& front) noexcept { front_ = glm::normalize(front); }
        void setWorldUp(const glm::vec3& worldUp) noexcept { world_up_ = glm::normalize(worldUp); }

        const glm::vec3& position() const noexcept { return position_; }
        const glm::vec3& front() const noexcept { return front_; }
        const glm::vec3& worldUp() const noexcept { return world_up_; }

    protected:
        glm::vec3 position_ = {0.0f, 0.0f, 3.0f};
        glm::vec3 front_ = {0.0f, 0.0f, -1.0f};
        glm::vec3 world_up_ = {0.0f, 1.0f, 0.0f};
    };
}



#endif //PBRRENDERER_CAMERA_HPP