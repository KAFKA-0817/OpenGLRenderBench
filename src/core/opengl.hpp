//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_OPENGL_HPP
#define PBRRENDERER_OPENGL_HPP

#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace core {
    class OpenGLContext {
    public:
        OpenGLContext();
        ~OpenGLContext();

        OpenGLContext(const OpenGLContext&) = delete;
        OpenGLContext& operator=(const OpenGLContext&) = delete;
        OpenGLContext(OpenGLContext&&) = delete;
        OpenGLContext& operator=(OpenGLContext&&) = delete;

        static void loadGlad();
        static void setDefaultWindowHints();
        static void enableDebugOutput();
    };
}

#endif //PBRRENDERER_OPENGL_HPP