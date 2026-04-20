//
// Created by kaede on 2026/4/19.
//

#include "FileDialog.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>

#include <array>
#endif

namespace core {
    std::optional<std::filesystem::path> FileDialog::openGltfModel() {
#ifdef _WIN32
        std::array<wchar_t, 32768> file_name{};

        OPENFILENAMEW dialog{};
        dialog.lStructSize = sizeof(dialog);
        dialog.lpstrFilter = L"glTF Model (*.gltf;*.glb)\0*.gltf;*.glb\0All Files (*.*)\0*.*\0";
        dialog.lpstrFile = file_name.data();
        dialog.nMaxFile = static_cast<DWORD>(file_name.size());
        dialog.lpstrTitle = L"Open glTF Model";
        dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameW(&dialog) == TRUE) {
            return std::filesystem::path(file_name.data());
        }
#endif
        return std::nullopt;
    }
}
