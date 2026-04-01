//
// Created by kaede on 2026/3/30.
//

#include "Texture2D.hpp"

#include <stdexcept>
#include <vector>

namespace renderer {

    Texture2D::~Texture2D() {
        destroy();
    }

    Texture2D::Texture2D(Texture2D&& other) noexcept
        : id_(other.id_) {
        other.id_ = 0;
    }

    Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
        if (this != &other) {
            destroy();
            id_ = other.id_;
            other.id_ = 0;
        }
        return *this;
    }

    void Texture2D::destroy() noexcept {
        if (id_ != 0) {
            glDeleteTextures(1, &id_);
            id_ = 0;
        }
    }

    void Texture2D::bind(GLuint unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id_);
    }

    static Texture2D createSolidTexture(unsigned char r,
                                        unsigned char g,
                                        unsigned char b,
                                        unsigned char a) {
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        unsigned char data[4] = {r, g, b, a};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
        return Texture2D(tex);
    }

    Texture2D Texture2D::createWhite1x1() {
        return createSolidTexture(255, 255, 255, 255);
    }

    Texture2D Texture2D::createBlack1x1() {
        return createSolidTexture(0, 0, 0, 255);
    }

    Texture2D Texture2D::createFlatNormal1x1() {
        return createSolidTexture(128, 128, 255, 255);
    }

    Texture2D Texture2D::createFromPixels(const unsigned char* pixels, int width, int height, int channels, bool srgb)
    {
        if (!pixels || width <= 0 || height <= 0) {
            throw std::runtime_error("Texture2D::createFromPixels invalid input.");
        }

        GLenum format = GL_RGBA;
        GLenum internal_format = GL_RGBA8;

        switch (channels) {
            case 1:
                format = GL_RED;
                internal_format = GL_R8;
                break;
            case 2:
                format = GL_RG;
                internal_format = GL_RG8;
                break;
            case 3:
                format = GL_RGB;
                internal_format = srgb ? GL_SRGB8 : GL_RGB8;
                break;
            case 4:
                format = GL_RGBA;
                internal_format = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
                break;
            default:
                throw std::runtime_error("Unsupported channel count in Texture2D::createFromPixels.");
        }

        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     internal_format,
                     width,
                     height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     pixels);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
        return Texture2D(tex);
    }

    Texture2D Texture2D::createFromChannel(const unsigned char* pixels, int width, int height, int channels, int channel_index) {
        if (!pixels || width <= 0 || height <= 0 || channel_index < 0 || channel_index >= channels) {
            throw std::runtime_error("Texture2D::createFromChannel invalid input.");
        }

        std::vector<unsigned char> channel_pixels(width * height);
        for (int i = 0; i < width * height; ++i) {
            channel_pixels[i] = pixels[i * channels + channel_index];
        }

        return createFromPixels(channel_pixels.data(), width, height, 1, false);
    }

} // namespace renderer