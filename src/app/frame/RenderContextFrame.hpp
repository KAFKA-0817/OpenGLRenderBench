//
// Created by kaede on 2026/4/8.
//

#ifndef PBRRENDERER_RENDERCONTEXTFRAME_HPP
#define PBRRENDERER_RENDERCONTEXTFRAME_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include <glm/vec3.hpp>

#include "../../core/noncopyable.hpp"

namespace renderer {
    inline constexpr std::size_t kMaxPointLights = 64;

    struct DirectionalLightData {
        glm::vec3 direction{0.0f, -1.0f, 0.0f};
        glm::vec3 color{1.0f, 1.0f, 1.0f};
        float intensity = 1.0f;
        bool valid = false;
    };

    struct PointLightData {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        float range = 10.0f;
        glm::vec3 color{1.0f, 1.0f, 1.0f};
        float intensity = 1.0f;
    };

    struct RenderContext {
        glm::vec3 camera_position{0.0f, 0.0f, 0.0f};
        DirectionalLightData directional_light{};
        std::array<PointLightData, kMaxPointLights> point_lights{};
        std::uint32_t point_light_count = 0;
        float exposure = 1.0f;
    };

    class RenderContextFrame : public core::NonCopyable {
    public:
        RenderContextFrame() = default;
        ~RenderContextFrame() = default;
        RenderContextFrame(RenderContextFrame&&) noexcept = delete;
        RenderContextFrame& operator=(RenderContextFrame&&) noexcept = delete;

        void beginFrame() noexcept { context_ = {}; }

        RenderContext& writable() noexcept { return context_; }
        const RenderContext& context() const noexcept { return context_; }

    private:
        RenderContext context_{};
    };
} // renderer

#endif //PBRRENDERER_RENDERCONTEXTFRAME_HPP
