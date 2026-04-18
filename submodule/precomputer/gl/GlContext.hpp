//
// Created by kaede on 2026/4/18.
//

#ifndef PRECOMPUTER_GLCONTEXT_HPP
#define PRECOMPUTER_GLCONTEXT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLContext {
public:
    GLContext();
    ~GLContext();
    GLContext(const GLContext&) = delete;
    GLContext& operator=(const GLContext&) = delete;
    GLContext(GLContext&&) = delete;
    GLContext& operator=(GLContext&&) = delete;

    GLFWwindow* window() const noexcept { return window_; }
    void makeCurrent() const noexcept;

private:
    static void setWindowHints();

private:
    GLFWwindow* window_ = nullptr;
};



#endif //PRECOMPUTER_GLCONTEXT_HPP
