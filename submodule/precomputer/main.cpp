//
// Created by kaede on 2026/4/18.
//

#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <ktx.h>

#include "EnvironmentBaker.hpp"
#include "KtxWriter.hpp"
#include "gl/GlContext.hpp"

int main() {
    const char* success_message = ktxErrorString(KTX_SUCCESS);
    if (success_message == nullptr) {
        std::cerr << "ktxErrorString returned null." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        GLContext context;
        context.makeCurrent();

        const std::filesystem::path hdr_path =
            R"(C:\Users\kaede\codes\cpp\Renderer\OpenGLRenderBench\assets\ibl\SunnySky\SunnySky.hdr)";
        const std::filesystem::path environment_output_path = hdr_path.parent_path() / "SunnySky_env.ktx2";
        const std::filesystem::path irradiance_output_path = hdr_path.parent_path() / "SunnySky_irradiance.ktx2";
        const std::filesystem::path prefilter_output_path = hdr_path.parent_path() / "SunnySky_prefilter.ktx2";

        EnvironmentBaker baker;
        const EnvironmentBakeResult result = baker.bakeFromHdr(hdr_path, 512, 32, 512, 10);
        KTXWriter::writeEnvironmentCubemap(result.environment, environment_output_path);
        KTXWriter::writeEnvironmentCubemap(result.irradiance, irradiance_output_path);
        KTXWriter::writePrefilterCubemap(result.prefilter, prefilter_output_path);

        std::cout << "libktx link check passed: " << success_message << std::endl;
        std::cout << "Environment cubemap baked from HDR: " << hdr_path << std::endl;
        std::cout << "Environment face size: " << result.environment.face_size << std::endl;
        std::cout << "Irradiance face size: " << result.irradiance.face_size << std::endl;
        std::cout << "Prefilter base face size: " << result.prefilter.base_face_size
                  << ", mip count: " << result.prefilter.mip_count << std::endl;
        std::cout << "Environment KTX2 written to: " << environment_output_path << std::endl;
        std::cout << "Irradiance KTX2 written to: " << irradiance_output_path << std::endl;
        std::cout << "Prefilter KTX2 written to: " << prefilter_output_path << std::endl;
    } catch (const std::exception& exception) {
        std::cerr << "Environment precompute failed: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
