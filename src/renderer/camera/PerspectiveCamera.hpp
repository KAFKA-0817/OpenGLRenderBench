//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_PERSPECTIVECAMERA_HPP
#define PBRRENDERER_PERSPECTIVECAMERA_HPP
#include "Camera.hpp"

namespace renderer {
    class PerspectiveCamera :public Camera{
    public:
        PerspectiveCamera() = default;
        PerspectiveCamera(const glm::vec3& position, const glm::vec3& front, const glm::vec3& world_up, float fov, float aspect, float z_near, float z_far);
        ~PerspectiveCamera() override = default;
        PerspectiveCamera(PerspectiveCamera&& other) noexcept = default;
        PerspectiveCamera& operator=(PerspectiveCamera&& other) noexcept = default;
        glm::mat4 getProjectionMatrix() const override;

        void setFov(float fov) noexcept { fov_ = fov; }
        void setAspect(float aspect) noexcept { aspect_ = aspect; }
        void setNearPlane(float zNear) noexcept { z_near_ = zNear; }
        void setFarPlane(float zFar) noexcept { z_far_ = zFar; }
        float fov() const noexcept { return fov_; }
        float aspect() const noexcept { return aspect_; }
        float nearPlane() const noexcept { return z_near_; }
        float farPlane() const noexcept { return z_far_; }

    private:
        float fov_ = 45.0f;
        float aspect_ = 1920.0f/1080.0f;
        float z_near_ = 0.1f;
        float z_far_ = 100.0f;
    };
}




#endif //PBRRENDERER_PERSPECTIVECAMERA_HPP