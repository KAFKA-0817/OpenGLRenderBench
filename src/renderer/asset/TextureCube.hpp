//
// Created by kaede on 2026/4/19.
//

#ifndef PBRRENDERER_TEXTURECUBE_HPP
#define PBRRENDERER_TEXTURECUBE_HPP

#include <filesystem>

#include "../../core/noncopyable.hpp"
#include "../../core/opengl.hpp"

namespace renderer {
    class TextureCube : public core::NonCopyable {
    public:
        TextureCube() = default;
        explicit TextureCube(GLuint id) : id_(id) {}
        ~TextureCube();

        TextureCube(TextureCube&& other) noexcept;
        TextureCube& operator=(TextureCube&& other) noexcept;

        void bind(GLuint unit) const;
        GLuint id() const noexcept { return id_; }

        static TextureCube createFromKtx2(const std::filesystem::path& path);

    private:
        void destroy() noexcept;

    private:
        GLuint id_ = 0;
    };
}

#endif //PBRRENDERER_TEXTURECUBE_HPP
