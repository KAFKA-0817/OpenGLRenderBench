//
// Created by kaede on 2026/4/18.
//

#include "KtxWriter.hpp"

#include <string>
#include <stdexcept>
#include <cstring>

#include <ktx.h>

namespace {
    constexpr ktx_uint32_t kVkFormatR32G32Sfloat = 103u;
    constexpr ktx_uint32_t kVkFormatR32G32B32A32Sfloat = 109u;

    void checkKtxResult(const KTX_error_code result, const char* operation) {
        if (result != KTX_SUCCESS) {
            throw std::runtime_error(std::string(operation) + " failed: " + ktxErrorString(result));
        }
    }
}

void KTXWriter::writeBrdfLut(const BrdfLutBakeResult& image, const std::filesystem::path& output_path) {
    if (image.width == 0 || image.height == 0) {
        throw std::runtime_error("BRDF LUT image is empty.");
    }

    // The GPU texture may stay alive for direct preview; KTX writing only consumes the CPU copy.
    const std::size_t expected_pixel_count =
        static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height) * 2u;
    if (image.pixels_rg32f.size() != expected_pixel_count) {
        throw std::runtime_error("BRDF LUT pixel buffer size does not match width * height * 2.");
    }

    if (output_path.has_parent_path()) {
        std::filesystem::create_directories(output_path.parent_path());
    }

    const ktxTextureCreateInfo create_info = {
        0,
        kVkFormatR32G32Sfloat,
        nullptr,
        image.width,
        image.height,
        1,
        2,
        1,
        1,
        1,
        KTX_FALSE,
        KTX_FALSE
    };

    ktxTexture2* texture = nullptr;
    checkKtxResult(
        ktxTexture2_Create(&create_info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture),
        "ktxTexture2_Create"
    );
    ktxTexture* base_texture = reinterpret_cast<ktxTexture*>(texture);

    try {
        checkKtxResult(
            ktxTexture_SetImageFromMemory(
                base_texture,
                0,
                0,
                0,
                reinterpret_cast<const ktx_uint8_t*>(image.pixels_rg32f.data()),
                static_cast<ktx_size_t>(image.pixels_rg32f.size() * sizeof(float))
            ),
            "ktxTexture_SetImageFromMemory"
        );

        checkKtxResult(
            ktxTexture_WriteToNamedFile(base_texture, output_path.string().c_str()),
            "ktxTexture_WriteToNamedFile"
        );
    } catch (...) {
        ktxTexture_Destroy(base_texture);
        throw;
    }

    ktxTexture_Destroy(base_texture);
}

void KTXWriter::writeEnvironmentCubemap(const EnvironmentCubemapBakeResult& image, const std::filesystem::path& output_path) {
    if (image.face_size == 0) {
        throw std::runtime_error("Environment cubemap image is empty.");
    }

    const std::size_t expected_pixel_count =
        static_cast<std::size_t>(image.face_size) *
        static_cast<std::size_t>(image.face_size) *
        6u * 4u;
    if (image.pixels_rgba32f.size() != expected_pixel_count) {
        throw std::runtime_error("Environment cubemap pixel buffer size does not match face_size * face_size * 6 * 4.");
    }

    if (output_path.has_parent_path()) {
        std::filesystem::create_directories(output_path.parent_path());
    }

    const ktxTextureCreateInfo create_info = {
        0,
        kVkFormatR32G32B32A32Sfloat,
        nullptr,
        image.face_size,
        image.face_size,
        1,
        2,
        1,
        1,
        6,
        KTX_FALSE,
        KTX_FALSE
    };

    ktxTexture2* texture = nullptr;
    checkKtxResult(
        ktxTexture2_Create(&create_info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture),
        "ktxTexture2_Create"
    );
    ktxTexture* base_texture = reinterpret_cast<ktxTexture*>(texture);

    const std::size_t face_pixel_count =
        static_cast<std::size_t>(image.face_size) *
        static_cast<std::size_t>(image.face_size) * 4u;
    const ktx_size_t face_byte_size = static_cast<ktx_size_t>(face_pixel_count * sizeof(float));

    try {
        ktx_uint8_t* texture_data = ktxTexture_GetData(base_texture);
        if (texture_data == nullptr) {
            throw std::runtime_error("ktxTexture_GetData returned null.");
        }

        for (ktx_uint32_t face = 0; face < 6; ++face) {
            const float* face_pixels = image.pixels_rgba32f.data() + face * face_pixel_count;
            ktx_size_t offset = 0;
            checkKtxResult(
                ktxTexture_GetImageOffset(base_texture, 0, 0, face, &offset),
                "ktxTexture_GetImageOffset"
            );
            std::memcpy(
                texture_data + offset,
                reinterpret_cast<const ktx_uint8_t*>(face_pixels),
                static_cast<std::size_t>(face_byte_size)
            );
        }

        checkKtxResult(
            ktxTexture_WriteToNamedFile(base_texture, output_path.string().c_str()),
            "ktxTexture_WriteToNamedFile"
        );
    } catch (...) {
        ktxTexture_Destroy(base_texture);
        throw;
    }

    ktxTexture_Destroy(base_texture);
}

void KTXWriter::writePrefilterCubemap(const PrefilterCubemapBakeResult& image, const std::filesystem::path& output_path) {
    if (image.base_face_size == 0 || image.mip_count == 0 || image.mips.size() != image.mip_count) {
        throw std::runtime_error("Prefilter cubemap image is empty.");
    }

    if (output_path.has_parent_path()) {
        std::filesystem::create_directories(output_path.parent_path());
    }

    const ktxTextureCreateInfo create_info = {
        0,
        kVkFormatR32G32B32A32Sfloat,
        nullptr,
        image.base_face_size,
        image.base_face_size,
        1,
        2,
        image.mip_count,
        1,
        6,
        KTX_FALSE,
        KTX_FALSE
    };

    ktxTexture2* texture = nullptr;
    checkKtxResult(
        ktxTexture2_Create(&create_info, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture),
        "ktxTexture2_Create"
    );
    ktxTexture* base_texture = reinterpret_cast<ktxTexture*>(texture);

    try {
        ktx_uint8_t* texture_data = ktxTexture_GetData(base_texture);
        if (texture_data == nullptr) {
            throw std::runtime_error("ktxTexture_GetData returned null.");
        }

        for (ktx_uint32_t mip = 0; mip < image.mip_count; ++mip) {
            const auto& mip_image = image.mips[mip];
            if (mip_image.face_size == 0) {
                throw std::runtime_error("Prefilter cubemap mip is empty.");
            }

            const std::size_t face_pixel_count =
                static_cast<std::size_t>(mip_image.face_size) *
                static_cast<std::size_t>(mip_image.face_size) * 4u;
            const std::size_t expected_pixel_count = face_pixel_count * 6u;
            if (mip_image.pixels_rgba32f.size() != expected_pixel_count) {
                throw std::runtime_error("Prefilter cubemap mip pixel buffer has unexpected size.");
            }

            const ktx_size_t face_byte_size = static_cast<ktx_size_t>(face_pixel_count * sizeof(float));
            for (ktx_uint32_t face = 0; face < 6; ++face) {
                const float* face_pixels = mip_image.pixels_rgba32f.data() + face * face_pixel_count;
                ktx_size_t offset = 0;
                checkKtxResult(
                    ktxTexture_GetImageOffset(base_texture, mip, 0, face, &offset),
                    "ktxTexture_GetImageOffset"
                );
                std::memcpy(
                    texture_data + offset,
                    reinterpret_cast<const ktx_uint8_t*>(face_pixels),
                    static_cast<std::size_t>(face_byte_size)
                );
            }
        }

        checkKtxResult(
            ktxTexture_WriteToNamedFile(base_texture, output_path.string().c_str()),
            "ktxTexture_WriteToNamedFile"
        );
    } catch (...) {
        ktxTexture_Destroy(base_texture);
        throw;
    }

    ktxTexture_Destroy(base_texture);
}
