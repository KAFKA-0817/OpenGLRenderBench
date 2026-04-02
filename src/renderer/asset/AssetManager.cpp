//
// Created by kaede on 2026/4/1.
//

#include "AssetManager.hpp"

#include "importer/GltfImporter.hpp"
#include "importer/RuntimeAssetBuilder.hpp"

namespace renderer {
    std::string AssetManager::normalizePathKey(const std::filesystem::path& path) {
        std::error_code ec;
        auto normalized = std::filesystem::weakly_canonical(path, ec);
        if (ec) {
            normalized = path.lexically_normal();
        }
        return normalized.string();
    }

    Model& AssetManager::loadModel(const std::filesystem::path& path) {
        std::string key = normalizePathKey(path);

        if (model_cache_.find(key) == model_cache_.end()) {
            auto imported = GltfImporter::import_model_from_file(path);
            auto model = RuntimeAssetBuilder::buildFromImported(imported);
            model_cache_[key] = std::make_unique<Model>(std::move(model));
        }

        return *model_cache_[key];
    }

    void AssetManager::unloadModel(const std::filesystem::path& path) {
        std::string key = normalizePathKey(path);
        model_cache_.erase(key);
    }

    void AssetManager::clear() {
        model_cache_.clear();
    }
} // renderer