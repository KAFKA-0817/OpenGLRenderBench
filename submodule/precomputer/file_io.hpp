//
// Created by kaede on 2026/3/26.
//

#ifndef PBRRENDERER_FILE_IO_HPP
#define PBRRENDERER_FILE_IO_HPP
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

inline std::string read_text_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("[file_io] File does not exist: " + path.string() );
    }

    const std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("[file_io] Failed to open file: " + path.string() );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


#endif //PBRRENDERER_FILE_IO_HPP