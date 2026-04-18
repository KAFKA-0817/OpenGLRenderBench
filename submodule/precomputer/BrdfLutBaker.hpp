//
// Created by kaede on 2026/4/18.
//

#ifndef PRECOMPUTER_BRDFLUTBAKER_HPP
#define PRECOMPUTER_BRDFLUTBAKER_HPP

#include <cstdint>
#include <filesystem>
#include <vector>

#include "gl/GlContext.hpp"

struct BrdfLutBakeResult {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    unsigned int texture = 0;
    std::vector<float> pixels_rg32f;

    void releaseTexture() {
        if (texture != 0) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
    }
};

class BrdfLutBaker {
public:
    BrdfLutBaker() = default;
    BrdfLutBakeResult bake(std::uint32_t size = 512) const;

private:
    static unsigned int createColorTexture(std::uint32_t size);
    static unsigned int createFramebuffer(unsigned int color_texture);
    static unsigned int createFullscreenQuadVao(unsigned int& vertex_buffer);
    static std::filesystem::path shaderDirectory();
};



#endif //PRECOMPUTER_BRDFLUTBAKER_HPP
