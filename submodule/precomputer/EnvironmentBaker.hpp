//
// Created by kaede on 2026/4/18.
//

#ifndef PRECOMPUTER_ENVIRONMENTBAKER_HPP
#define PRECOMPUTER_ENVIRONMENTBAKER_HPP

#include <cstdint>
#include <filesystem>
#include <vector>
#include <array>

#include <glm/mat4x4.hpp>

struct EnvironmentCubemapBakeResult {
    std::uint32_t face_size = 0;
    std::vector<float> pixels_rgba32f;
};

struct PrefilterCubemapMip {
    std::uint32_t face_size = 0;
    std::vector<float> pixels_rgba32f;
};

struct PrefilterCubemapBakeResult {
    std::uint32_t base_face_size = 0;
    std::uint32_t mip_count = 0;
    std::vector<PrefilterCubemapMip> mips;
};

struct EnvironmentBakeResult {
    EnvironmentCubemapBakeResult environment;
    EnvironmentCubemapBakeResult irradiance;
    PrefilterCubemapBakeResult prefilter;
};

class EnvironmentBaker {
public:
    EnvironmentBakeResult bakeFromHdr(const std::filesystem::path& hdr_path,
                                      std::uint32_t environment_size = 512,
                                      std::uint32_t irradiance_size = 32,
                                      std::uint32_t prefilter_size = 128,
                                      std::uint32_t prefilter_mip_count = 5) const;

private:
    struct GpuCubemap {
        unsigned int texture = 0;
        std::uint32_t face_size = 0;
        std::uint32_t mip_count = 1;
    };

    static unsigned int loadHdrTexture(const std::filesystem::path& hdr_path);
    static GpuCubemap bakeEnvironmentCubemapGpu(unsigned int hdr_texture, std::uint32_t face_size);
    static GpuCubemap bakeIrradianceCubemapGpu(unsigned int environment_map, std::uint32_t face_size);
    static GpuCubemap bakePrefilterCubemapGpu(unsigned int environment_map, std::uint32_t face_size, std::uint32_t mip_count);
    static EnvironmentCubemapBakeResult readCubemap(unsigned int cubemap, std::uint32_t face_size);
    static PrefilterCubemapBakeResult readPrefilterCubemap(unsigned int cubemap, std::uint32_t base_face_size, std::uint32_t mip_count);
    static unsigned int createCubemapTexture(std::uint32_t face_size, std::uint32_t mip_count = 1);
    static unsigned int createCaptureFramebuffer(std::uint32_t face_size, unsigned int& depth_rbo);
    static unsigned int createCubeVao(unsigned int& vertex_buffer);
    static std::array<glm::mat4, 6> captureViews();
    static std::filesystem::path shaderDirectory();
};

#endif //PRECOMPUTER_ENVIRONMENTBAKER_HPP
