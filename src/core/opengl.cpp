//
// Created by kaede on 2026/3/30.
//

#include <stdexcept>
#include "opengl.hpp"

#include <iostream>

namespace {

    const char* debugSourceToString(GLenum source) {
        switch (source) {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WindowSystem";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "ShaderCompiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "ThirdParty";
            case GL_DEBUG_SOURCE_APPLICATION: return "Application";
            case GL_DEBUG_SOURCE_OTHER: return "Other";
            default: return "Unknown";
        }
    }

    const char* debugTypeToString(GLenum type) {
        switch (type) {
            case GL_DEBUG_TYPE_ERROR: return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DeprecatedBehavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UndefinedBehavior";
            case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
            case GL_DEBUG_TYPE_MARKER: return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP: return "PushGroup";
            case GL_DEBUG_TYPE_POP_GROUP: return "PopGroup";
            case GL_DEBUG_TYPE_OTHER: return "Other";
            default: return "Unknown";
        }
    }

    const char* debugSeverityToString(GLenum severity) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH: return "High";
            case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
            case GL_DEBUG_SEVERITY_LOW: return "Low";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
            default: return "Unknown";
        }
    }

    void APIENTRY openglDebugCallback(GLenum source,
                                      GLenum type,
                                      GLuint id,
                                      GLenum severity,
                                      GLsizei length,
                                      const GLchar* message,
                                      const void* userParam) {
        (void)length;
        (void)userParam;

        // 过滤notification
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
            return;
        }

        std::cerr
            << "[OpenGL Debug] "
            << "[Source: " << debugSourceToString(source) << "] "
            << "[Type: " << debugTypeToString(type) << "] "
            << "[Severity: " << debugSeverityToString(severity) << "] "
            << "[ID: " << id << "] "
            << message << '\n';
    }

} // namespace

namespace core {
    void glfwErrorCallback(int error, const char* description) {
        std::cerr << "[GLFW Error] (" << error << ") "
                  << (description ? description : "Unknown error")
                  << '\n';
    }

    OpenGLContext::OpenGLContext() {
        glfwSetErrorCallback(glfwErrorCallback);

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW.");
        }
    }

    OpenGLContext::~OpenGLContext() {
        glfwTerminate();
    }

    void OpenGLContext::loadGlad() {
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            throw std::runtime_error("Failed to initialize GLAD.");
        }
    }

    void OpenGLContext::setDefaultWindowHints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    }

    void OpenGLContext::enableDebugOutput() {
        GLint flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

        if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
            std::cerr << "[OpenGL Debug] Debug context is not available.\n";
            return;
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglDebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                              0, nullptr, GL_TRUE);

        std::cerr << "[OpenGL Debug] Debug output enabled.\n";
    }
}
