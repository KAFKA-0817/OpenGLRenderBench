//
// Created by kaede on 2026/3/27.
//

#include "FrameBuffer.hpp"

#include <stdexcept>

namespace renderer {
    FrameBuffer::FrameBuffer(int width, int height,int color_attachment_count, GLenum color_internal_format)
        :width_(width),height_(height),color_attachment_count_(color_attachment_count),color_internal_format_(color_internal_format)
    {
        invalidate();
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
        if (this != &other) {
            destroy();

            fbo_ = other.fbo_;
            color_attachments_ = std::move(other.color_attachments_);
            depth_rbo_ = other.depth_rbo_;
            width_ = other.width_;
            height_ = other.height_;
            color_attachment_count_ = other.color_attachment_count_;
            color_internal_format_ = other.color_internal_format_;

            other.fbo_ = 0;
            other.depth_rbo_ = 0;
            other.width_ = 0;
            other.height_ = 0;
            other.color_attachment_count_ = 1;
        }
        return *this;
    }

    FrameBuffer::~FrameBuffer() {
        destroy();
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : fbo_(other.fbo_),
      color_attachments_(std::move(other.color_attachments_)),
      depth_rbo_(other.depth_rbo_),
      width_(other.width_),
      height_(other.height_),
      color_attachment_count_(other.color_attachment_count_),
      color_internal_format_(other.color_internal_format_) {
        other.fbo_ = 0;
        other.depth_rbo_ = 0;
        other.width_ = 0;
        other.height_ = 0;
        other.color_attachment_count_ = 1;
    }

    void FrameBuffer::invalidate() {
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        color_attachments_.resize(color_attachment_count_);
        glGenTextures(color_attachment_count_, color_attachments_.data());

        for (int i = 0; i < color_attachment_count_; ++i) {
            glBindTexture(GL_TEXTURE_2D, color_attachments_[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, color_internal_format_,
                         width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + i,
                                   GL_TEXTURE_2D,
                                   color_attachments_[i],
                                   0);
        }

        std::vector<GLenum> draw_buffers(color_attachment_count_);
        for (int i = 0; i < color_attachment_count_; ++i) {
            draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(color_attachment_count_, draw_buffers.data());

        glGenRenderbuffers(1, &depth_rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depth_rbo_);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            destroy();
            throw std::runtime_error("Failed to create framebuffer.");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::destroy() noexcept
    {
        if (!color_attachments_.empty()) {
            glDeleteTextures(static_cast<GLsizei>(color_attachments_.size()),
                             color_attachments_.data());
            color_attachments_.clear();
        }

        if (depth_rbo_ != 0) {
            glDeleteRenderbuffers(1, &depth_rbo_);
            depth_rbo_ = 0;
        }

        if (fbo_ != 0) {
            glDeleteFramebuffers(1, &fbo_);
            fbo_ = 0;
        }
    }

    void FrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER,fbo_);
        glViewport(0,0,width_,height_);
    }

    void FrameBuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    void FrameBuffer::resize(int width, int height)
    {
        destroy();
        width_ = width;
        height_ = height;
        invalidate();
    }

    void FrameBuffer::blitDepth(const FrameBuffer& src, const FrameBuffer& dst) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src.fbo_);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.fbo_);

        glBlitFramebuffer(
            0, 0, src.width(), src.height(),
            0, 0, dst.width(), dst.height(),
            GL_DEPTH_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
