//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_MATERIAL_HPP
#define PBRRENDERER_MATERIAL_HPP
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include "../../core/noncopyable.hpp"
#include "../camera/Camera.hpp"

namespace renderer {
    enum class RenderPath {
        Deferred,
        Forward
    };

    struct RenderContext {
        glm::vec3 camera_position{0.0f, 0.0f, 0.0f};

        // 第一版先只放一盏方向光
        glm::vec3 light_direction{-0.2f, -1.0f, -0.3f};
        glm::vec3 light_color{1.0f, 1.0f, 1.0f};
    };

    class Material : public core::NonCopyable {
    public:
        Material()=default;
        virtual ~Material() = default;
        Material(Material&&) = delete;
        Material& operator=(Material&&) = delete;

        virtual RenderPath renderPath() const noexcept = 0;
    };
}

#endif //PBRRENDERER_MATERIAL_HPP
