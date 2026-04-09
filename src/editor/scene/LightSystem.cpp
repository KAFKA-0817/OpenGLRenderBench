//
// Created by kaede on 2026/4/8.
//

#include "LightSystem.hpp"

#include <algorithm>

#include <glm/geometric.hpp>

namespace editor {
    namespace {
        constexpr glm::vec3 kDefaultDirectionalLightDirection{-0.2f, -1.0f, -0.3f};

        glm::vec3 computeForwardDirection(const TransformComponent& transform) {
            const glm::vec3 rotated = transform.forwardDirection();
            const float length = glm::length(rotated);
            if (length <= 1e-4f) {
                return glm::normalize(kDefaultDirectionalLightDirection);
            }
            return rotated / length;
        }
    }

    void LightSystem::writeLights(renderer::RenderContextFrame& frame_context, const Scene& scene) {
        auto& context = frame_context.writable();

        for (const auto& entity : scene.entities()) {
            const auto* directional_light = scene.tryGetDirectionalLight(entity);
            if (!directional_light || !directional_light->enabled) {
                continue;
            }

            context.directional_light.valid = true;
            context.directional_light.color = glm::vec3(
                std::max(directional_light->color.r, 0.0f),
                std::max(directional_light->color.g, 0.0f),
                std::max(directional_light->color.b, 0.0f)
            );
            context.directional_light.intensity = std::max(directional_light->intensity, 0.0f);

            if (const auto* transform = scene.tryGetTransform(entity)) {
                context.directional_light.direction = computeForwardDirection(*transform);
            } else {
                context.directional_light.direction = glm::normalize(kDefaultDirectionalLightDirection);
            }
            break;
        }

        for (const auto& entity : scene.entities()) {
            if (context.point_light_count >= renderer::kMaxPointLights) {
                break;
            }

            const auto* point_light = scene.tryGetPointLight(entity);
            if (!point_light || !point_light->enabled) {
                continue;
            }

            auto& dst = context.point_lights[context.point_light_count++];
            const auto* transform = scene.tryGetTransform(entity);
            dst.color = glm::vec3(
                std::max(point_light->color.r, 0.0f),
                std::max(point_light->color.g, 0.0f),
                std::max(point_light->color.b, 0.0f)
            );
            dst.intensity = std::max(point_light->intensity, 0.0f);
            dst.range = std::max(point_light->range, 0.0f);
            dst.position = transform ? transform->position : glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }
} // editor
