//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_FORWARDPASS_HPP
#define PBRRENDERER_FORWARDPASS_HPP
#include <vector>

#include "RenderItem.hpp"
#include "RenderPass.hpp"
#include "../core/FrameBuffer.hpp"

namespace renderer {
    class ForwardPass : public RenderPass {
    public:
        ForwardPass() = default;

        void execute(const std::vector<RenderItem>& forward_items,
                     const Camera& camera,
                     RenderContext& render_context,
                     const FrameBuffer& target_framebuffer);
    };
} // renderer

#endif //PBRRENDERER_FORWARDPASS_HPP