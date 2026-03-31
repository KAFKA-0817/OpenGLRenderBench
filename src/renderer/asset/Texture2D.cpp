//
// Created by kaede on 2026/3/30.
//

#include "Texture2D.hpp"

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

} // namespace renderer