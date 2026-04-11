//
// Created by Codex on 2026/4/11.
//

#ifndef PBRRENDERER_SHADOWFRAMEBUFFER_HPP
#define PBRRENDERER_SHADOWFRAMEBUFFER_HPP

#include "../../core/noncopyable.hpp"
#include "../../core/opengl.hpp"

namespace renderer {
    class ShadowFrameBuffer : public core::NonCopyable {
    public:
        ShadowFrameBuffer(int width, int height);
        ~ShadowFrameBuffer();
        ShadowFrameBuffer(ShadowFrameBuffer&& other) noexcept;
        ShadowFrameBuffer& operator=(ShadowFrameBuffer&& other) noexcept;

        void bind() const;
        void unbind() const;
        void resize(int width, int height);

        int width() const noexcept { return width_; }
        int height() const noexcept { return height_; }
        GLuint colorAttachment() const noexcept { return color_attachment_; }

    private:
        void invalidate();
        void destroy() noexcept;

    private:
        GLuint fbo_ = 0;
        GLuint color_attachment_ = 0;
        GLuint depth_rbo_ = 0;
        int width_ = 0;
        int height_ = 0;
    };
}

#endif //PBRRENDERER_SHADOWFRAMEBUFFER_HPP
