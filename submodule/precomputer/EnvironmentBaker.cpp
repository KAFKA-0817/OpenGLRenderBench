//
// Created by kaede on 2026/4/18.
//

#include "EnvironmentBaker.hpp"

#include <array>
#include <stdexcept>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <stb_image.h>

#include "gl/Shader.hpp"

namespace {
    constexpr float kCubeVertices[] = {
        // back
        -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // left
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        // right
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
        // front
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // top
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // bottom
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
}

EnvironmentCubemapBakeResult EnvironmentBaker::bakeFromHdr(const std::filesystem::path& hdr_path, const std::uint32_t face_size) const {
    if (face_size == 0) {
        throw std::runtime_error("Environment cubemap face size must be greater than zero.");
    }

    const unsigned int hdr_texture = loadHdrTexture(hdr_path);
    const unsigned int cube_texture = createCubemapTexture(face_size);
    unsigned int depth_rbo = 0;
    const unsigned int framebuffer = createCaptureFramebuffer(face_size, depth_rbo);
    unsigned int cube_vbo = 0;
    const unsigned int cube_vao = createCubeVao(cube_vbo);

    const std::filesystem::path shader_dir = shaderDirectory();
    Shader shader(shader_dir / "equirect_to_cubemap.vert", shader_dir / "equirect_to_cubemap.frag");
    shader.use();
    shader.setInt("uEquirectangularMap", 0);

    const glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const std::array<glm::mat4, 6> views = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    shader.setMat4("uProjection", projection);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, static_cast<GLsizei>(face_size), static_cast<GLsizei>(face_size));
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    glBindVertexArray(cube_vao);

    for (std::uint32_t face = 0; face < 6; ++face) {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
            cube_texture,
            0
        );

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Environment baker framebuffer is incomplete.");
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.setMat4("uView", views[face]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    EnvironmentCubemapBakeResult result;
    result.face_size = face_size;
    result.pixels_rgba32f.resize(
        static_cast<std::size_t>(face_size) *
        static_cast<std::size_t>(face_size) *
        6u * 4u,
        0.0f
    );

    glBindTexture(GL_TEXTURE_CUBE_MAP, cube_texture);
    const std::size_t face_pixel_count = static_cast<std::size_t>(face_size) * static_cast<std::size_t>(face_size) * 4u;
    for (std::uint32_t face = 0; face < 6; ++face) {
        float* face_pixels = result.pixels_rgba32f.data() + face * face_pixel_count;
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, GL_FLOAT, face_pixels);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDeleteTextures(1, &hdr_texture);
    glDeleteTextures(1, &cube_texture);
    glDeleteRenderbuffers(1, &depth_rbo);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteBuffers(1, &cube_vbo);
    glDeleteVertexArrays(1, &cube_vao);

    return result;
}

unsigned int EnvironmentBaker::loadHdrTexture(const std::filesystem::path& hdr_path) {
    if (hdr_path.empty()) {
        throw std::runtime_error("HDR path is empty.");
    }
    if (!std::filesystem::exists(hdr_path)) {
        throw std::runtime_error("HDR file does not exist: " + hdr_path.string());
    }
    if (!stbi_is_hdr(hdr_path.string().c_str())) {
        throw std::runtime_error("Input image is not HDR: " + hdr_path.string());
    }

    stbi_set_flip_vertically_on_load(0);

    int width = 0;
    int height = 0;
    int channels = 0;
    float* pixels = stbi_loadf(hdr_path.string().c_str(), &width, &height, &channels, 0);
    if (!pixels) {
        throw std::runtime_error("Failed to load HDR image: " + hdr_path.string());
    }

    GLenum format = GL_RGB;
    GLenum internal_format = GL_RGB16F;
    if (channels == 1) {
        format = GL_RED;
        internal_format = GL_R16F;
    } else if (channels == 3) {
        format = GL_RGB;
        internal_format = GL_RGB16F;
    } else if (channels == 4) {
        format = GL_RGBA;
        internal_format = GL_RGBA16F;
    } else {
        stbi_image_free(pixels);
        throw std::runtime_error("Unsupported HDR channel count.");
    }

    unsigned int texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        width,
        height,
        0,
        format,
        GL_FLOAT,
        pixels
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(pixels);
    return texture;
}

unsigned int EnvironmentBaker::createCubemapTexture(const std::uint32_t face_size) {
    unsigned int texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    for (unsigned int face = 0; face < 6; ++face) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
            0,
            GL_RGBA16F,
            static_cast<GLsizei>(face_size),
            static_cast<GLsizei>(face_size),
            0,
            GL_RGBA,
            GL_FLOAT,
            nullptr
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

unsigned int EnvironmentBaker::createCaptureFramebuffer(const std::uint32_t face_size, unsigned int& depth_rbo) {
    unsigned int framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT24,
        static_cast<GLsizei>(face_size),
        static_cast<GLsizei>(face_size)
    );
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return framebuffer;
}

unsigned int EnvironmentBaker::createCubeVao(unsigned int& vertex_buffer) {
    unsigned int vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return vao;
}

std::filesystem::path EnvironmentBaker::shaderDirectory() {
    return std::filesystem::path(__FILE__).parent_path() / "shader";
}
