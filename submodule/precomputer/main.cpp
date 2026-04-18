//
// Created by kaede on 2026/4/18.
//
#include <cstdlib>
#include <iostream>

#include <ktx.h>

#include "BrdfLutBaker.hpp"

int main() {
    const char* success_message = ktxErrorString(KTX_SUCCESS);
    if (success_message == nullptr) {
        std::cerr << "ktxErrorString returned null." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        GLContext context;
        context.makeCurrent();
        BrdfLutBaker baker;
        const BrdfLutBakeResult result = baker.bake(256);

        std::cout << "libktx link check passed: " << success_message << std::endl;
        std::cout << "BRDF LUT skeleton bake passed: "
                  << result.width << "x" << result.height
                  << ", pixels=" << result.pixels_rg32f.size()
                  << std::endl;
    } catch (const std::exception& exception) {
        std::cerr << "Precomputer startup failed: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
