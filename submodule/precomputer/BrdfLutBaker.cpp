//
// Created by kaede on 2026/4/18.
//

#include "BrdfLutBaker.hpp"

#include <stdexcept>

#include "gl/Shader.hpp"

BrdfLutBakeResult BrdfLutBaker::bake(const std::uint32_t size) const {
    const unsigned int color_texture = createColorTexture(size);
    const unsigned int framebuffer = createFramebuffer(color_texture);
    unsigned int quad_buffer = 0;
    const unsigned int quad_vao = createFullscreenQuadVao(quad_buffer);

    const std::filesystem::path shader_dir = shaderDirectory();
    Shader shader(shader_dir / "brdf_lut.vert", shader_dir / "brdf_lut.frag");

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, static_cast<GLsizei>(size), static_cast<GLsizei>(size));
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader.use();
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    BrdfLutBakeResult result;
    result.width = size;
    result.height = size;
    result.texture = color_texture;
    result.pixels_rg32f.resize(static_cast<std::size_t>(size) * static_cast<std::size_t>(size) * 2u, 0.0f);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(
        0,
        0,
        static_cast<GLsizei>(size),
        static_cast<GLsizei>(size),
        GL_RG,
        GL_FLOAT,
        result.pixels_rg32f.data()
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteBuffers(1, &quad_buffer);
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteFramebuffers(1, &framebuffer);

    return result;
}

unsigned int BrdfLutBaker::createColorTexture(const std::uint32_t size) {
    unsigned int texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RG16F,
        static_cast<GLsizei>(size),
        static_cast<GLsizei>(size),
        0,
        GL_RG,
        GL_HALF_FLOAT,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

unsigned int BrdfLutBaker::createFramebuffer(const unsigned int color_texture) {
    unsigned int framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &framebuffer);
        throw std::runtime_error("BRDF LUT framebuffer is incomplete.");
    }

    return framebuffer;
}

unsigned int BrdfLutBaker::createFullscreenQuadVao(unsigned int& vertex_buffer) {
    constexpr float kQuadVertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
    };

    unsigned int vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return vao;
}

std::filesystem::path BrdfLutBaker::shaderDirectory() {
    return std::filesystem::path(__FILE__).parent_path() / "shader";
}
