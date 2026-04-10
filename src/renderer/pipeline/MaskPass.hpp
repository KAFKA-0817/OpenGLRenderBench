//
// Created by kaede on 2026/4/10.
//

#ifndef PBRRENDERER_MASKPASS_HPP
#define PBRRENDERER_MASKPASS_HPP
#include "RenderItem.hpp"
#include "RenderPass.hpp"
#include "../camera/Camera.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"

namespace renderer {

class MaskPass : public RenderPass{
public:
    MaskPass(int width, int height);
    void execute(const RenderItem& item, const Camera& camera);
    GLuint colorAttachment() const noexcept { return framebuffer_.colorAttachment(0); }
    void resize(int width, int height) { framebuffer_.resize(width, height); }

    const FrameBuffer& framebuffer() const noexcept { return framebuffer_; }
    bool reloadShader() { return mask_shader_.reload(); }
    Shader& shader() noexcept { return mask_shader_; }
    const Shader& shader() const noexcept { return mask_shader_; }
private:
    FrameBuffer framebuffer_;
    Shader mask_shader_;
};

} // renderer

#endif //PBRRENDERER_MASKPASS_HPP
