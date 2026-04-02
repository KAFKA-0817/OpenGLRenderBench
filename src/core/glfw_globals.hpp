//
// Created by kaede on 2026/4/1.
//

#ifndef PBRRENDERER_GLFW_GLOBALS_HPP
#define PBRRENDERER_GLFW_GLOBALS_HPP
#include <unordered_map>
#include "opengl.hpp"

namespace renderer {
    class Window;
    class OrbitController;
}

namespace core {
    inline std::unordered_map<GLFWwindow*,renderer::Window*> glfw_window_map{};
    inline std::unordered_map<GLFWwindow*,renderer::OrbitController*> glfw_orbit_controller_map{};
}

#endif //PBRRENDERER_GLFW_GLOBALS_HPP