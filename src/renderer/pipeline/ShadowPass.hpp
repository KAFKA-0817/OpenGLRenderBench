//
// Created by kaede on 2026/4/11.
//

#ifndef PBRRENDERER_SHADOWPASS_HPP
#define PBRRENDERER_SHADOWPASS_HPP
#include "RenderItem.hpp"
#include "../../core/Log.hpp"
#include "../camera/OrthographicCamera.hpp"
#include "../core/ShadowFrameBuffer.hpp"
#include "../core/Shader.hpp"

namespace renderer {

class ShadowPass {
public:
    ShadowPass(int width, int height);
    void resize(int width, int height) { frame_buffer_.resize(width, height); }
    void setLightCamera(const Camera& view_camera, const glm::vec3& direction);
    void execute(const std::vector<RenderItem>& render_items) const;
    glm::mat4 getLightSpaceMatrix() const { return camera_.getProjectionMatrix() * camera_.getViewMatrix();}

    GLuint colorAttachment() const noexcept { return frame_buffer_.colorAttachment(); }
    bool reloadShader() {
        core::Log::getInstance().write("Shader","Shadow-pass shader reloaded");
        return shader_.reload();
    }
    Shader& shader() noexcept { return shader_; }
    const Shader& shader() const noexcept { return shader_; }
private:
    ShadowFrameBuffer frame_buffer_;
    Shader shader_;
    OrthographicCamera camera_;
};

} // renderer

#endif //PBRRENDERER_SHADOWPASS_HPP
