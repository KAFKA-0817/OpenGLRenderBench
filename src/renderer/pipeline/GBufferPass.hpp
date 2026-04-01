//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_GBUFFERPASS_HPP
#define PBRRENDERER_GBUFFERPASS_HPP
#include <vector>

#include "RenderItem.hpp"
#include "RenderPass.hpp"
#include "../core/Shader.hpp"
#include "../core/FrameBuffer.hpp"
#include "../camera/Camera.hpp"

namespace renderer {
    class GBufferPass :public RenderPass {
    public:
        GBufferPass(int width, int height);

        void resize(int width, int height);

        void execute(const std::vector<RenderItem>& deferred_items,
                     const Camera& camera);

        const FrameBuffer& framebuffer() const noexcept { return framebuffer_; }
        GLuint gPosition() const noexcept { return framebuffer_.colorAttachment(0); }
        GLuint gNormal() const noexcept { return framebuffer_.colorAttachment(1); }
        GLuint gAlbedo() const noexcept { return framebuffer_.colorAttachment(2); }
        GLuint gMaterial() const noexcept { return framebuffer_.colorAttachment(3); }
        GLuint gEmissive() const noexcept { return framebuffer_.colorAttachment(4); }

        int width() const noexcept { return framebuffer_.width(); }
        int height() const noexcept { return framebuffer_.height(); }

        bool reloadShader();

    private:
        FrameBuffer framebuffer_;
        Shader pbr_gbuffer_shader_;
    };
} // renderer

#endif //PBRRENDERER_GBUFFERPASS_HPP