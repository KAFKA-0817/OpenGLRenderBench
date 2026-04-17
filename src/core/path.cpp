//
// Created by kaede on 2026/3/30.
//

#include "path.hpp"

#include <stdexcept>

namespace core {
    namespace {
        std::filesystem::path g_project_root;
        bool g_project_root_initialized = false;

        bool isProjectRoot(const std::filesystem::path& candidate) {
            std::error_code ec;
            return std::filesystem::exists(candidate / "CMakeLists.txt", ec) &&
                   std::filesystem::is_directory(candidate / "src", ec) &&
                   std::filesystem::is_directory(candidate / "assets", ec) &&
                   std::filesystem::is_directory(candidate / "assets" / "shaders", ec) &&
                   std::filesystem::is_directory(candidate / "assets" / "models", ec);
        }

        std::filesystem::path findProjectRoot(const std::filesystem::path& executable_path) {
            const auto normalized_path = ProjectPaths::normalize(executable_path);
            auto current = normalized_path;

            std::error_code ec;
            if (!std::filesystem::is_directory(current, ec)) {
                current = current.parent_path();
            }

            while (!current.empty()) {
                if (isProjectRoot(current)) {
                    return current;
                }

                const auto parent = current.parent_path();
                if (parent == current) {
                    break;
                }
                current = parent;
            }

            throw std::runtime_error("Failed to locate project root from executable path: " + normalized_path.string());
        }
    }

    void ProjectPaths::initialize(const std::filesystem::path& executable_path) {
        g_project_root = findProjectRoot(executable_path);
        g_project_root_initialized = true;
    }

    const std::filesystem::path& ProjectPaths::root() {
        if (!g_project_root_initialized) {
            throw std::logic_error("ProjectPaths::initialize must be called before ProjectPaths::root");
        }
        return g_project_root;
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
