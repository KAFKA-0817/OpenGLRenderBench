//
// Created by kaede on 2026/3/30.
//

#include "path.hpp"

namespace core {
    std::filesystem::path ProjectPaths::root() {
        return R"(C:\Users\kaede\codes\cpp\Renderer\OpenGLRenderBench)";
    }

    std::filesystem::path ProjectPaths::assets() {
        return root() / "assets";
    }

    std::filesystem::path ProjectPaths::shaders() {
        return assets() / "shaders";
    }

    std::filesystem::path ProjectPaths::textures() {
        return assets() / "textures";
    }

    std::filesystem::path ProjectPaths::models() {
        return assets() / "models";
    }

    std::filesystem::path ProjectPaths::shader(const std::string& filename) {
        return shaders() / filename;
    }

    std::filesystem::path ProjectPaths::texture(const std::string& filename) {
        return textures() / filename;
    }

    std::filesystem::path ProjectPaths::model(const std::string& filename) {
        return models() / filename;
    }

    std::filesystem::path ProjectPaths::normalize(const std::filesystem::path& path) {
        std::error_code ec;
        auto normalized = std::filesystem::weakly_canonical(path, ec);
        if (ec) {
            normalized = path.lexically_normal();
        }
        return normalized;
    }
}
