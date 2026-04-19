//
// Created by kaede on 2026/4/18.
//

#ifndef PRECOMPUTER_ENVIRONMENTBAKER_HPP
#define PRECOMPUTER_ENVIRONMENTBAKER_HPP

#include <cstdint>
#include <filesystem>
#include <vector>

struct EnvironmentCubemapBakeResult {
    std::uint32_t face_size = 0;
    std::vector<float> pixels_rgba32f;
};

class EnvironmentBaker {
public:
    EnvironmentCubemapBakeResult bakeFromHdr(const std::filesystem::path& hdr_path, std::uint32_t face_size = 512) const;

private:
    static unsigned int loadHdrTexture(const std::filesystem::path& hdr_path);
    static unsigned int createCubemapTexture(std::uint32_t face_size);
    static unsigned int createCaptureFramebuffer(std::uint32_t face_size, unsigned int& depth_rbo);
    static unsigned int createCubeVao(unsigned int& vertex_buffer);
    static std::filesystem::path shaderDirectory();
};

#endif //PRECOMPUTER_ENVIRONMENTBAKER_HPP
