//
// Created by kaede on 2026/4/12.
//

#ifndef OPENGLRENDERBENCH_ZPREPASS_HPP
#define OPENGLRENDERBENCH_ZPREPASS_HPP

#include <vector>

#include "RenderItem.hpp"
#include "RenderPass.hpp"
#include "../../core/Log.hpp"
#include "../camera/Camera.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"

namespace renderer {
    class ZPrePass : public RenderPass {
    public:
        ZPrePass(int width, int height);

        void resize(int width, int height) { framebuffer_.resize(width, height); }
        void execute(const std::vector<RenderItem>& render_items, const Camera& camera);

        const FrameBuffer& framebuffer() const noexcept { return framebuffer_; }
        bool reloadShader() {
            core::Log::getInstance().write("Shader","ZPre shader reloaded");
            return shader_.reload();
        }

    private:
        FrameBuffer framebuffer_;
        Shader shader_;
    };
} // renderer

#endif //OPENGLRENDERBENCH_ZPREPASS_HPP
