//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_MATERIAL_HPP
#define PBRRENDERER_MATERIAL_HPP
#include "../../core/noncopyable.hpp"

namespace renderer {
    enum class RenderPath {
        Deferred,
        Forward
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
