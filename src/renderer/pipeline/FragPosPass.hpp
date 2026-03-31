//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_FRAGPOSPASS_HPP
#define PBRRENDERER_FRAGPOSPASS_HPP
#include "RenderItem.hpp"
#include "../../core/opengl.hpp"
#include "../core/Shader.hpp"
#include "../asset/Model.hpp"
#include "../core/FrameBuffer.hpp"
#include "../camera/Camera.hpp"
#include "RenderPass.hpp"

namespace renderer {
    class FragPosPass :public RenderPass {
    public:
        FragPosPass(int width, int height);

        void resize(int width, int height);

        int width() const noexcept { return framebuffer_.width(); }
        int height() const noexcept { return framebuffer_.height(); }

        void execute(const std::vector<RenderItem>& items,
                     const Camera& camera);

        GLuint colorAttachment() const noexcept { return framebuffer_.colorAttachment(); }

        bool reloadShader();

    private:
        FrameBuffer framebuffer_;
        Shader shader_;
    };
} // renderer

#endif //PBRRENDERER_FRAGPOSPASS_HPP