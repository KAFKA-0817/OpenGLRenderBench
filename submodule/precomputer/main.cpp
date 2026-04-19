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
        const std::filesystem::path output_path = hdr_path.parent_path() / "SunnySky.ktx2";

        EnvironmentBaker baker;
        const EnvironmentCubemapBakeResult result = baker.bakeFromHdr(hdr_path, 1024);
        KTXWriter::writeEnvironmentCubemap(result, output_path);

        std::cout << "libktx link check passed: " << success_message << std::endl;
        std::cout << "Environment cubemap baked from HDR: " << hdr_path << std::endl;
        std::cout << "Face size: " << result.face_size << std::endl;
        std::cout << "KTX2 written to: " << output_path << std::endl;
    } catch (const std::exception& exception) {
        std::cerr << "Environment precompute failed: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
