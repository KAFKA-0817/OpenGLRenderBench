//
// Created by kaede on 2026/4/1.
//

#ifndef PBRRENDERER_ASSETMANAGER_HPP
#define PBRRENDERER_ASSETMANAGER_HPP
#include <filesystem>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "Model.hpp"
#include "../../core/noncopyable.hpp"
#include "../../core/ThreadPool.hpp"
#include "importer/ImportedModelData.hpp"

namespace renderer {

    enum class ModelState {
        LoadingCPU,
        Ready,
        Unloaded
    };

    struct ModelRecord {
        ModelState state = ModelState::Unloaded;
        std::unique_ptr<Model> model;
    };

    class AssetManager :public core::NonCopyable{
    public:
        AssetManager();
        ~AssetManager() = default;

        AssetManager(AssetManager&&) = delete;
        AssetManager& operator=(AssetManager&&) = delete;

        Model* loadModel(const std::filesystem::path& path);
        void unloadModel(const std::filesystem::path& path);
        void clear();

        Model* tryGetModel(const std::filesystem::path& path);
        void requestModel(const std::filesystem::path& path);
        void pumpUploads();

    private:
        static std::string normalizePathKey(const std::filesystem::path& path);
    private:
        //若以值对象存储cache，触发rehash时对象地址可能不稳定
        core::ThreadPool thread_pool_;

        std::unordered_map<std::string,ModelRecord> model_cache_;
        std::mutex model_cache_mutex_;
        std::queue<std::pair<std::string,ImportedModelData>> imported_queue_;
        std::mutex imported_queue_mutex_;
    };
} // renderer

#endif //PBRRENDERER_ASSETMANAGER_HPP