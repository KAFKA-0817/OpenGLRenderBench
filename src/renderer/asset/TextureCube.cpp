//
// Created by kaede on 2026/4/19.
//

#include "TextureCube.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

#include <ktx.h>

namespace renderer {
    namespace {
        void checkKtxResult(const KTX_error_code result, const char* operation) {
            if (result != KTX_SUCCESS) {
                throw std::runtime_error(std::string(operation) + " failed: " + ktxErrorString(result));
            }
        }
    }

    TextureCube::~TextureCube() {
        destroy();
    }

    TextureCube::TextureCube(TextureCube&& other) noexcept
        : id_(other.id_), mip_count_(other.mip_count_) {
        other.id_ = 0;
        other.mip_count_ = 0;
    }

    TextureCube& TextureCube::operator=(TextureCube&& other) noexcept {
        if (this != &other) {
            destroy();
            id_ = other.id_;
            mip_count_ = other.mip_count_;
            other.id_ = 0;
            other.mip_count_ = 0;
        }
        return *this;
    }

    void TextureCube::destroy() noexcept {
        if (id_ != 0) {
            glDeleteTextures(1, &id_);
            id_ = 0;
        }
        mip_count_ = 0;
    }

    void TextureCube::bind(const GLuint unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
    }

    TextureCube TextureCube::createFromKtx2(const std::filesystem::path& path) {
        if (path.empty()) {
            throw std::runtime_error("TextureCube::createFromKtx2 received an empty path.");
        }
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("TextureCube::createFromKtx2 missing file: " + path.string());
        }

        ktxTexture* ktx_texture = nullptr;
        checkKtxResult(
            ktxTexture_CreateFromNamedFile(
                path.string().c_str(),
                KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                &ktx_texture
            ),
            "ktxTexture_CreateFromNamedFile"
        );

        GLuint texture_id = 0;
        GLenum target = GL_NONE;
        GLenum gl_error = GL_NO_ERROR;
        const int mip_count = static_cast<int>(std::max(1u, ktx_texture->numLevels));

        try {
            checkKtxResult(
                ktxTexture_GLUpload(ktx_texture, &texture_id, &target, &gl_error),
                "ktxTexture_GLUpload"
            );

            if (target != GL_TEXTURE_CUBE_MAP) {
                glDeleteTextures(1, &texture_id);
                texture_id = 0;
                throw std::runtime_error("TextureCube::createFromKtx2 only supports GL_TEXTURE_CUBE_MAP assets.");
            }

            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mip_count - 1);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                            mip_count > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        } catch (...) {
            ktxTexture_Destroy(ktx_texture);
            throw;
        }

        ktxTexture_Destroy(ktx_texture);
        return TextureCube(texture_id, mip_count);
    }
}
