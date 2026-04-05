//
// Created by kaede on 2026/4/1.
//

#include "AssetManager.hpp"

#include "importer/GltfImporter.hpp"
#include "importer/RuntimeAssetBuilder.hpp"

namespace renderer {
    AssetManager::AssetManager() :thread_pool_(8){

    }

    std::string AssetManager::normalizePathKey(const std::filesystem::path& path)
    {
        std::error_code ec;
        auto normalized = std::filesystem::weakly_canonical(path, ec);
        if (ec) {
            normalized = path.lexically_normal();
        }
        return normalized.string();
    }

    Model* AssetManager::loadModel(const std::filesystem::path& path) {
        std::string key = normalizePathKey(path);

        if (model_cache_.find(key) == model_cache_.end()) {
            auto imported = GltfImporter::import_model_from_file(path);
            auto model = RuntimeAssetBuilder::buildFromImported(imported);
            model_cache_[key] = {
                ModelState::Ready,
                std::make_unique<Model>(std::move(model))
            };
        }

        return model_cache_[key].model.get();
    }

    void AssetManager::unloadModel(const std::filesystem::path& path) {
        std::string key = normalizePathKey(path);
        model_cache_.erase(key);
    }

    void AssetManager::clear() {
        model_cache_.clear();
    }

    Model* AssetManager::tryGetModel(const std::filesystem::path& path) {
        auto key = normalizePathKey(path);
        std::lock_guard lock(model_cache_mutex_);

        auto it = model_cache_.find(key);
        if (it != model_cache_.end()) {
            auto& record = it->second;
            if (record.state == ModelState::Ready && record.model) {
                return record.model.get();
            }
        }

        return nullptr;
    }

    void AssetManager::requestModel(const std::filesystem::path& path) {
        auto key = normalizePathKey(path);

        {
            std::lock_guard lock(model_cache_mutex_);
            auto it = model_cache_.find(key);
            if (it!=model_cache_.end() && it->second.state!=ModelState::Unloaded)
                return;

            model_cache_[key].state = ModelState::LoadingCPU;
            model_cache_[key].model.reset();
        }

        thread_pool_.enqueue([this,key,path]() {
            auto imported = GltfImporter::import_model_from_file(path);
            {
                std::lock_guard lock(imported_queue_mutex_);
                imported_queue_.emplace(key,std::move(imported));
            }
        });
    }

    void AssetManager::pumpUploads() {
        std::vector<std::pair<std::string, ImportedModelData>> pending;

        {
            std::lock_guard lock(imported_queue_mutex_);
            while (!imported_queue_.empty()) {
                pending.emplace_back(std::move(imported_queue_.front()));
                imported_queue_.pop();
            }
        }

        for (auto& [key, imported] : pending) {
            auto model = RuntimeAssetBuilder::buildFromImported(imported);

            std::lock_guard lock(model_cache_mutex_);
            auto& record = model_cache_[key];
            record.state = ModelState::Ready;
            record.model = std::make_unique<Model>(std::move(model));
        }
    }
} // renderer