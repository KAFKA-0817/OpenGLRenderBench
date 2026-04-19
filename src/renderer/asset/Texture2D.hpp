//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_TEXTURE2D_HPP
#define PBRRENDERER_TEXTURE2D_HPP
#include <filesystem>

#include "../../core/opengl.hpp"
#include "../../core/noncopyable.hpp"

namespace renderer {
    class Texture2D : public core::NonCopyable {
    public:
        Texture2D() = default;
        explicit Texture2D(GLuint id) : id_(id) {}
        ~Texture2D();

        Texture2D(Texture2D&& other) noexcept;
        Texture2D& operator=(Texture2D&& other) noexcept;

        void bind(GLuint unit) const;
        GLuint id() const noexcept { return id_; }

        //测试生成贴图
        static Texture2D createWhite1x1();
        static Texture2D createBlack1x1();
        static Texture2D createFlatNormal1x1();

        static Texture2D createFromPixels(const unsigned char* pixels,
                                          int width,
                                          int height,
                                          int channels,
                                          bool srgb);

        static Texture2D createFromKtx2(const std::filesystem::path& path);

        static Texture2D createFromChannel(const unsigned char* pixels,
                                           int width,
                                           int height,
                                           int channels,
                                           int channel_index);


    private:
        void destroy() noexcept;

    private:
        GLuint id_ = 0;
    };

}


#endif //PBRRENDERER_TEXTURE2D_HPP
