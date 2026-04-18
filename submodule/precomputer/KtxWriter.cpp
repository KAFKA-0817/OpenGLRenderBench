//
// Created by kaede on 2026/4/18.
//

#include "KtxWriter.hpp"

#include <string>
#include <stdexcept>

#include <ktx.h>

namespace {
    constexpr ktx_uint32_t kVkFormatR32G32Sfloat = 103u;

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
