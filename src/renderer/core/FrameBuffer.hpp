//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_FRAMEBUFFER_HPP
#define PBRRENDERER_FRAMEBUFFER_HPP
#include <vector>

#include "../../core/noncopyable.hpp"
#include "../../core/opengl.hpp"


namespace renderer {
    class FrameBuffer : public core::NonCopyable {
    public:
        FrameBuffer(int width, int height,int color_attachment_count = 1, GLenum color_internal_format = GL_RGBA16F);
        ~FrameBuffer();
        FrameBuffer(FrameBuffer&& other) noexcept;
        FrameBuffer& operator=(FrameBuffer&& other) noexcept;

        void bind() const;
        void unbind() const;
        void resize(int width, int height);
        int width() const noexcept { return width_; }
        int height() const noexcept { return height_; }

        GLuint colorAttachment(int index = 0) const noexcept { return color_attachments_[index]; }
        GLuint depthAttachment() const noexcept { return depth_rbo_; }
        int colorAttachmentCount() const noexcept { return static_cast<int>(color_attachments_.size()); }
        static void blitDepth(const FrameBuffer& src, const FrameBuffer& dst);

    private:
        void invalidate();
        void destroy() noexcept;

    private:
        GLuint fbo_ = 0;
        std::vector<GLuint> color_attachments_;
        GLuint depth_rbo_ = 0;
        int color_attachment_count_ = 1;
        GLenum color_internal_format_ = GL_RGBA16F;

        int width_;
        int height_;
    };
}


#endif //PBRRENDERER_FRAMEBUFFER_HPP
