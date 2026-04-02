//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_VERTEX_HPP
#define PBRRENDERER_VERTEX_HPP
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace renderer {
    struct Vertex {
        glm::vec3 position{0.f,0.f,0.f};
        glm::vec3 normal{0.f,0.f,0.f};
        glm::vec2 texCoord{0.f,0.f};
        glm::vec3 tangent{0.f,0.f,0.f};
        glm::vec3 bitangent{0.f,0.f,0.f};
    };

    enum class AlphaMode {
        Opaque,
        Mask,
        Blend
    };
}

#endif //PBRRENDERER_VERTEX_HPP