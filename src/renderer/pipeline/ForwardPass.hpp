//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_FORWARDPASS_HPP
#define PBRRENDERER_FORWARDPASS_HPP
#include <vector>

#include "RenderItem.hpp"
#include "RenderPass.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"
#include "../../app/frame/RenderContextFrame.hpp"
#include "../camera/Camera.hpp"

namespace renderer {
    class ForwardPass : public RenderPass {
    public:
        ForwardPass();

        void execute(const std::vector<RenderItem>& forward_items,
                     const Camera& camera,
                     const RenderContext& render_context,
                     GLuint shadow_map,
                     const glm::mat4& light_space_matrix,
                     const FrameBuffer& target_framebuffer);

        void executeTransparentPbr(const std::vector<RenderItem>& transparent_items,
                                   const Camera& camera,
                                   const RenderContext& render_context,
                                   const FrameBuffer& target_framebuffer);

        bool reloadShader();

    private:
        Shader transparent_pbr_shader_;
    };
} // renderer

#endif //PBRRENDERER_FORWARDPASS_HPP
