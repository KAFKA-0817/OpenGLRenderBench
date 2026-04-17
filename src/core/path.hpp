//
// Created by kaede on 2026/3/30.
//

#ifndef PBRRENDERER_PATH_HPP
#define PBRRENDERER_PATH_HPP
#include <filesystem>
#include <string>

namespace core {
    class ProjectPaths {
    public:
        static void initialize(const std::filesystem::path& executable_path);

        static const std::filesystem::path& root();
        static std::filesystem::path assets();
        static std::filesystem::path shaders();
        static std::filesystem::path textures();
        static std::filesystem::path models();

        static std::filesystem::path shader(const std::string& filename);
        static std::filesystem::path texture(const std::string& filename);
        static std::filesystem::path model(const std::string& filename);

        static std::filesystem::path normalize(const std::filesystem::path& path);
    };
}

#endif //PBRRENDERER_PATH_HPP
