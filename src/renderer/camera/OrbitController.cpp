//
// Created by kaede on 2026/4/1.
//

#include "OrbitController.hpp"

#include <algorithm>

#include "../../core/glfw_globals.hpp"

namespace renderer {
    OrbitController::OrbitController(GLFWwindow* window, Camera* camera)
        :window_(window), camera_(camera)
    {
        core::glfw_orbit_controller_map[window_] = this;
        glfwSetScrollCallback(window_, scroll_callback);
        updateCamera();
    }

    OrbitController::~OrbitController() {
        1+1;
    }

    void OrbitController::reset() {
        target_= {0.0f, 0.0f, 0.0f};
        distance_ = 5.0f;
        yaw_ = glm::radians(0.0f);
        pitch_ = glm::radians(36.0f);
        first_mouse_ = true;
        pending_scroll_delta_ = 0.0f;
        updateCamera();
    }

    void OrbitController::applyOrbit(float delta_x, float delta_y) {
        yaw_ -= delta_x * rotate_speed_;
        pitch_ += delta_y * rotate_speed_;

        const float max_pitch = glm::radians(89.0f);
        pitch_ = std::clamp(pitch_, -max_pitch, max_pitch);
    }

    void OrbitController::applyZoom(float scroll_delta) {
        if (std::abs(scroll_delta) < 1e-6f) {
            return;
        }

        distance_ *= std::exp(-scroll_delta * zoom_speed_ * 0.1f);
        distance_ = std::clamp(distance_, min_distance_, max_distance_);
    }

    void OrbitController::applyPan(float delta_x, float delta_y) {
        const glm::vec3 position = camera_->position();
        const glm::vec3 front = glm::normalize(target_ - position);
        const glm::vec3 right = glm::normalize(glm::cross(front, camera_->worldUp()));
        const glm::vec3 up = glm::normalize(glm::cross(right, front));

        const float pan_factor = pan_speed_ * distance_;
        target_ += (-delta_x * pan_factor) * right;
        target_ += ( delta_y * pan_factor) * up;
    }

    void OrbitController::updateCamera() {
        const float cos_pitch = std::cos(pitch_);
        const glm::vec3 offset{
            distance_ * cos_pitch * std::sin(yaw_),
            distance_ * std::sin(pitch_),
            distance_ * cos_pitch * std::cos(yaw_)
        };

        const glm::vec3 position = target_ + offset;
        const glm::vec3 front = glm::normalize(target_ - position);

        camera_->setPosition(position);
        camera_->setFront(front);
    }

    void OrbitController::update() {
        double cursor_x = 0.0;
        double cursor_y = 0.0;
        glfwGetCursorPos(window_, &cursor_x, &cursor_y);
        if (first_mouse_) {
            last_x_ = cursor_x;
            last_y_ = cursor_y;
            first_mouse_ = false;
        }

        const float delta_x = static_cast<float>(cursor_x - last_x_);
        const float delta_y = static_cast<float>(cursor_y - last_y_);

        const bool left_pressed = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        const bool middle_pressed = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        const bool r_pressed = glfwGetKey(window_, GLFW_KEY_R) == GLFW_PRESS;

        if (left_pressed) {
            applyOrbit(delta_x, delta_y);
        }

        if (middle_pressed) {
            applyPan(delta_x, delta_y);
        }
        if (std::abs(pending_scroll_delta_) > 1e-6f) {
            applyZoom(pending_scroll_delta_);
            pending_scroll_delta_ = 0.0f;
        }
        if (r_pressed) {
            reset();
        }
        updateCamera();

        last_x_ = cursor_x;
        last_y_ = cursor_y;
    }

    void OrbitController::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        core::glfw_orbit_controller_map[window]->pending_scroll_delta_ += static_cast<float>(yoffset);
    }
} // renderer