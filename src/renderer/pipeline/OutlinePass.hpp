//
// Created by kaede on 2026/4/10.
//

#ifndef PBRRENDERER_OUTLINEPASS_HPP
#define PBRRENDERER_OUTLINEPASS_HPP
#include "RenderPass.hpp"
#include "../../core/Log.hpp"
#include "../asset/Mesh.hpp"
#include "../core/FrameBuffer.hpp"
#include "../core/Shader.hpp"

namespace renderer {

class OutlinePass : RenderPass{
public:
    OutlinePass(int width, int height);
    void execute(GLuint mask_attachment, GLuint scene_attachment);

    GLuint colorAttachment() const noexcept { return framebuffer_.colorAttachment(0); }

    void resize(int width, int height) { framebuffer_.resize(width, height); }
    bool reloadShader() {
        core::Log::getInstance().write("Shader","Outline shader reloaded");
        return outline_shader_.reload();
    }
    Shader& shader() noexcept { return outline_shader_; }
    const Shader& shader() const noexcept { return outline_shader_; }

private:
    FrameBuffer framebuffer_;
    Shader outline_shader_;
    Mesh screen_quad_;
};

} // renderer

#endif //PBRRENDERER_OUTLINEPASS_HPP
