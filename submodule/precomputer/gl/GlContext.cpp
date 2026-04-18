//
// Created by kaede on 2026/4/18.
//

#include "GlContext.hpp"

#include <stdexcept>

GLContext::GLContext(const int width, const int height, const char* title, const bool visible) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    setWindowHints(visible);

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }

    makeCurrent();

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD.");
    }

    glfwSwapInterval(0);
}

GLContext::~GLContext() {
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void GLContext::setWindowHints(const bool visible) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
}

void GLContext::makeCurrent() const noexcept{
    glfwMakeContextCurrent(window_);
}

void GLContext::swapBuffers() const noexcept {
    glfwSwapBuffers(window_);
}

void GLContext::pollEvents() const noexcept {
    glfwPollEvents();
}

bool GLContext::shouldClose() const noexcept {
    return glfwWindowShouldClose(window_) == GLFW_TRUE;
}
