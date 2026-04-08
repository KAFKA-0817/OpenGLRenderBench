//
// Created by kaede on 2026/4/8.
//

#ifndef PBRRENDERER_LIGHTSYSTEM_HPP
#define PBRRENDERER_LIGHTSYSTEM_HPP

#include "Scene.hpp"
#include "../../app/frame/RenderContextFrame.hpp"

namespace editor {
    class LightSystem {
    public:
        static void writeLights(renderer::RenderContextFrame& frame_context, const Scene& scene);
    };
} // editor

#endif //PBRRENDERER_LIGHTSYSTEM_HPP
