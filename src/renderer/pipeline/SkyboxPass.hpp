//
// Created by kaede on 2026/4/19.
//

#ifndef PBRRENDERER_SKYBOXPASS_HPP
#define PBRRENDERER_SKYBOXPASS_HPP

#include "../asset/Mesh.hpp"
#include "../asset/TextureCube.hpp"
#include "../camera/Camera.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"

namespace renderer {
    class SkyboxPass {
    public:
        SkyboxPass();

        void execute(const TextureCube& environment_map,
                     const Camera& camera,
                     const FrameBuffer& target_framebuffer);

        bool reloadShader();

    private:
        Mesh cube_;
        Shader shader_;
    };
}

#endif //PBRRENDERER_SKYBOXPASS_HPP
