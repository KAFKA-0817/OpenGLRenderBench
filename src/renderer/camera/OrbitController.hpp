//
// Created by kaede on 2026/4/1.
//

#ifndef PBRRENDERER_ORBITCONTROLLER_HPP
#define PBRRENDERER_ORBITCONTROLLER_HPP
#include "Camera.hpp"
#include "../../core/noncopyable.hpp"
#include "../../core/opengl.hpp"

namespace renderer {
    class OrbitController : public core::NonCopyable {
    public:
        OrbitController(GLFWwindow* window, Camera* camera);
        ~OrbitController();
        OrbitController(OrbitController&&) noexcept = delete;
        OrbitController& operator=(OrbitController&&) noexcept = delete;

        void update();
        void reset();
        void setDistance(float distance) noexcept { distance_ = distance; }
        void setRotateSpeed(float rotate_speed) noexcept { rotate_speed_ = rotate_speed; }
        void setPanSpeed(float pan_speed) noexcept { pan_speed_ = pan_speed; }
        void setZoomSpeed(float zoom_speed) noexcept { zoom_speed_ = zoom_speed; }
        void setMinDistance(float min_distance) noexcept { min_distance_ = min_distance; }
        void setMaxDistance(float max_distance) noexcept { max_distance_ = max_distance; }
        void setTarget(const glm::vec3& target) noexcept { target_ = target; }
        void setYaw(float yaw_degree) noexcept { yaw_ = glm::radians(yaw_degree); }
        void setPitch(float pitch_degree) noexcept { pitch_ = glm::radians(pitch_degree); }
        float yaw() const noexcept { return yaw_; }
        float pitch() const noexcept { return pitch_; }
        const glm::vec3& target() const noexcept { return target_; }
        float distance() const noexcept { return distance_; }
        float rotateSpeed() const noexcept { return rotate_speed_; }
        float panSpeed() const noexcept { return pan_speed_; }
        float zoomSpeed() const noexcept { return zoom_speed_; }
        float minDistance() const noexcept { return min_distance_; }
        float maxDistance() const noexcept { return max_distance_; }

    private:
        void updateCamera();
        void applyOrbit(float delta_x, float delta_y);
        void applyPan(float delta_x, float delta_y);
        void applyZoom(float scroll_delta);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    private:
        GLFWwindow* window_ = nullptr;
        Camera* camera_ = nullptr;

        glm::vec3 target_{0.0f, 0.0f, 0.0f};
        float distance_ = 5.0f;
        float yaw_ = glm::radians(0.0f);
        float pitch_ = glm::radians(10.0f);
        float rotate_speed_ = 0.005f;
        float pan_speed_ = 0.0015f;
        float zoom_speed_ = 0.8f;
        float min_distance_ = 0.2f;
        float max_distance_ = 100.0f;
        bool first_mouse_ = true;
        double last_x_ = 0.0;
        double last_y_ = 0.0;
        float pending_scroll_delta_ = 0.0f;
    };
} // renderer

#endif //PBRRENDERER_ORBITCONTROLLER_HPP