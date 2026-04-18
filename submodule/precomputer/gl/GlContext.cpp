//
// Created by kaede on 2026/4/18.
//

#include "GlContext.hpp"

#include <stdexcept>

GLContext::GLContext() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    setWindowHints();

    window_ = glfwCreateWindow(1, 1, "PrecomputerHiddenContext", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create hidden GLFW window.");
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

void GLContext::setWindowHints() {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
}

void GLContext::makeCurrent() const noexcept{
    glfwMakeContextCurrent(window_);
}
