//
// Created by kaede on 2026/3/26.
//

#include "Window.hpp"

#include <stdexcept>

#include "../../core/glfw_globals.hpp"

namespace renderer {
    Window::Window(int width, int height, const std::string& title)
    : width_(width), height_(height)
    {
        core::OpenGLContext::setDefaultWindowHints();

        window_ = glfwCreateWindow(width_, height_, title.c_str(), nullptr, nullptr);
        if (!window_) {
            throw std::runtime_error("Failed to create GLFW window.");
        }

        core::glfw_window_map[window_] = this;
        glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    }

    Window::~Window() {
        destroy();
    }

    Window::Window(Window&& other) noexcept
    : window_(other.window_),width_(other.width_), height_(other.height_)
    {
        other.window_ = nullptr;
        other.width_ = 0;
        other.height_ = 0;

        if (window_) {
            core::glfw_window_map[window_] = this;
            glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
        }
    }

    Window& Window::operator=(Window&& other) noexcept {
        if (this != &other) {
            destroy();
            window_ = other.window_;
            width_ = other.width_;
            height_ = other.height_;
            other.window_ = nullptr;
            other.width_ = 0;
            other.height_ = 0;

            if (window_) {
                core::glfw_window_map[window_] = this;
                glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
            }
        }

        return *this;
    }

    void Window::destroy() noexcept {
        if (window_) {
            core::glfw_window_map.erase(window_);
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
    }

    void Window::make_context_current() const {
        glfwMakeContextCurrent(window_);
    }

    bool Window::should_close() const {
        return glfwWindowShouldClose(window_);
    }

    void Window::swap_buffers() const {
        glfwSwapBuffers(window_);
    }

    void Window::poll_events() const {
        glfwPollEvents();
    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        auto self = core::glfw_window_map[window];
        if (self) {
            self->width_ = width;
            self->height_ = height;
            glViewport(0, 0, width, height);
        }
    }
}
