//
// Created by Codex on 2026/4/11.
//

#include "ShadowFrameBuffer.hpp"

#include <stdexcept>

namespace renderer {
    ShadowFrameBuffer::ShadowFrameBuffer(int width, int height)
        : width_(width), height_(height) {
        invalidate();
    }

    ShadowFrameBuffer::~ShadowFrameBuffer() {
        destroy();
    }

    ShadowFrameBuffer::ShadowFrameBuffer(ShadowFrameBuffer&& other) noexcept
        : fbo_(other.fbo_),
          color_attachment_(other.color_attachment_),
          depth_rbo_(other.depth_rbo_),
          width_(other.width_),
          height_(other.height_) {
        other.fbo_ = 0;
        other.color_attachment_ = 0;
        other.depth_rbo_ = 0;
        other.width_ = 0;
        other.height_ = 0;
    }

    ShadowFrameBuffer& ShadowFrameBuffer::operator=(ShadowFrameBuffer&& other) noexcept {
        if (this != &other) {
            destroy();

            fbo_ = other.fbo_;
            color_attachment_ = other.color_attachment_;
            depth_rbo_ = other.depth_rbo_;
            width_ = other.width_;
            height_ = other.height_;

            other.fbo_ = 0;
            other.color_attachment_ = 0;
            other.depth_rbo_ = 0;
            other.width_ = 0;
            other.height_ = 0;
        }
        return *this;
    }

    void ShadowFrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, width_, height_);
    }

    void ShadowFrameBuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowFrameBuffer::resize(int width, int height) {
        destroy();
        width_ = width;
        height_ = height;
        invalidate();
    }

    void ShadowFrameBuffer::invalidate() {
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        glGenTextures(1, &color_attachment_);
        glBindTexture(GL_TEXTURE_2D, color_attachment_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const GLfloat border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_, 0);

        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        glGenRenderbuffers(1, &depth_rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width_, height_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            destroy();
            throw std::runtime_error("Failed to create shadow framebuffer.");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowFrameBuffer::destroy() noexcept {
        if (color_attachment_ != 0) {
            glDeleteTextures(1, &color_attachment_);
            color_attachment_ = 0;
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
}
