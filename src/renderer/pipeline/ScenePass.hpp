//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_SCENEPASS_HPP
#define PBRRENDERER_SCENEPASS_HPP
#include "RenderItem.hpp"
#include "../../core/opengl.hpp"
#include "RenderPass.hpp"
#include "../material/Material.hpp"
#include "../core/FrameBuffer.hpp"

namespace renderer {
    class ScenePass :public RenderPass{
    public:
        ScenePass(int width, int height);

        void resize(int width, int height);

        int width() const noexcept { return framebuffer_.width(); }
        int height() const noexcept { return framebuffer_.height(); }

        void execute(const std::vector<RenderItem>& items,
                 const Camera& camera,
                 RenderContext& render_context,
                 float clear_r,
                 float clear_g,
                 float clear_b,
                 float clear_a);

        GLuint colorAttachment() const noexcept { return framebuffer_.colorAttachment(); }

    private:
        FrameBuffer framebuffer_;
    };
} // renderer

#endif //PBRRENDERER_SCENEPASS_HPP