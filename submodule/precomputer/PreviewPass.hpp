//
// Created by kaede on 2026/4/18.
//

#ifndef PRECOMPUTER_PREVIEWPASS_HPP
#define PRECOMPUTER_PREVIEWPASS_HPP

#include <filesystem>

#include "gl/Shader.hpp"

class PreviewPass {
public:
    PreviewPass();
    ~PreviewPass();

    PreviewPass(const PreviewPass&) = delete;
    PreviewPass& operator=(const PreviewPass&) = delete;
    PreviewPass(PreviewPass&&) = delete;
    PreviewPass& operator=(PreviewPass&&) = delete;

    void render(unsigned int texture, int viewport_width, int viewport_height) const;

private:
    static unsigned int createFullscreenQuad(unsigned int& vertex_buffer);
    static std::filesystem::path shaderDirectory();

private:
    Shader shader_;
    unsigned int vao_ = 0;
    unsigned int vertex_buffer_ = 0;
};

#endif //PRECOMPUTER_PREVIEWPASS_HPP
