//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_ORTHOGRAPHICCAMERA_HPP
#define PBRRENDERER_ORTHOGRAPHICCAMERA_HPP
#include "Camera.hpp"

namespace renderer {
    class OrthographicCamera :public Camera{
    public:
        OrthographicCamera() = default;
        OrthographicCamera(const glm::vec3& position, const glm::vec3& front, const glm::vec3& world_up, float left, float right, float bottom, float top, float z_near, float z_far);
        ~OrthographicCamera() override = default;
        OrthographicCamera(OrthographicCamera&& other) noexcept = default;
        OrthographicCamera& operator=(OrthographicCamera&& other) noexcept = default;

        glm::mat4 getProjectionMatrix() const override;

        void setLeft(float left) noexcept { left_ = left; }
        void setRight(float right) noexcept { right_ = right; }
        void setBottom(float bottom) noexcept { bottom_ = bottom; }
        void setTop(float top) noexcept { top_ = top; }
        float left() const noexcept { return left_; }
        float right() const noexcept { return right_; }
        float bottom() const noexcept { return bottom_; }
        float top() const noexcept { return top_; }

    private:
        float left_ = -1.0f;
        float right_ = 1.0f;
        float bottom_ = -1.0f;
        float top_ = 1.0f;
        float z_near_ = 0.1f;
        float z_far_ = 100.0f;
    };
}



#endif //PBRRENDERER_ORTHOGRAPHICCAMERA_HPP