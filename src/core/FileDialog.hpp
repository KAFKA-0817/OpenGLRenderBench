//
// Created by kaede on 2026/4/19.
//

#ifndef PBRRENDERER_FILEDIALOG_HPP
#define PBRRENDERER_FILEDIALOG_HPP

#include <filesystem>
#include <optional>

namespace core {
    class FileDialog {
    public:
        static std::optional<std::filesystem::path> openGltfModel();
    };
}

#endif //PBRRENDERER_FILEDIALOG_HPP
