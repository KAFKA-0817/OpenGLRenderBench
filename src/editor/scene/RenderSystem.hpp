//
// Created by kaede on 2026/4/3.
//

#ifndef PBRRENDERER_RENDERSYSTEM_HPP
#define PBRRENDERER_RENDERSYSTEM_HPP
#include "Scene.hpp"

namespace renderer {
    class Renderer;
}

namespace editor {
    class RenderSystem {
    public:
        static void renderScene(renderer::Renderer& renderer, const Scene& scene);
    };
}

#endif //PBRRENDERER_RENDERSYSTEM_HPP
