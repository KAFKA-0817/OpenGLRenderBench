//
// Created by kaede on 2026/4/18.
//
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <ktx.h>

#include "BrdfLutBaker.hpp"
#include "KtxWriter.hpp"
#include "PreviewPass.hpp"

int main() {
    const char* success_message = ktxErrorString(KTX_SUCCESS);
    if (success_message == nullptr) {
        std::cerr << "ktxErrorString returned null." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        GLContext context(800, 800, "BRDF LUT Preview", true);
        context.makeCurrent();
        BrdfLutBaker baker;
        BrdfLutBakeResult result = baker.bake(512);
        const std::filesystem::path output_path = std::filesystem::current_path() / "brdf_lut.ktx2";
        KTXWriter::writeBrdfLut(result, output_path);
        PreviewPass preview_pass;
        const float first_r = result.pixels_rg32f.empty() ? 0.0f : result.pixels_rg32f[0];
        const float first_g = result.pixels_rg32f.size() < 2 ? 0.0f : result.pixels_rg32f[1];

        std::cout << "libktx link check passed: " << success_message << std::endl;
        std::cout << "BRDF LUT draw pass passed: "
                  << result.width << "x" << result.height
                  << ", pixels=" << result.pixels_rg32f.size()
                  << ", first_rg=(" << first_r << ", " << first_g << ")"
                  << std::endl;
        std::cout << "BRDF LUT KTX2 written to: " << output_path << std::endl;

        while (!context.shouldClose()) {
            if (glfwGetKey(context.window(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(context.window(), GLFW_TRUE);
            }

            int framebuffer_width = 0;
            int framebuffer_height = 0;
            glfwGetFramebufferSize(context.window(), &framebuffer_width, &framebuffer_height);
            preview_pass.render(result.texture, framebuffer_width, framebuffer_height);
            context.swapBuffers();
            context.pollEvents();
        }

        result.releaseTexture();
    } catch (const std::exception& exception) {
        std::cerr << "Precomputer startup failed: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
