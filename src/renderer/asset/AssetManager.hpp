//
// Created by kaede on 2026/4/1.
//

#ifndef PBRRENDERER_ASSETMANAGER_HPP
#define PBRRENDERER_ASSETMANAGER_HPP
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "Model.hpp"
#include "../../core/noncopyable.hpp"

namespace renderer {
    class AssetManager :public core::NonCopyable{
    public:
        AssetManager() = default;
        ~AssetManager() = default;

        AssetManager(AssetManager&&) = delete;
        AssetManager& operator=(AssetManager&&) = delete;

        Model& loadModel(const std::filesystem::path& path);
        void unloadModel(const std::filesystem::path& path);
        void clear();

    private:
        static std::string normalizePathKey(const std::filesystem::path& path);
    private:
        //若以值对象存储cache，触发rehash时对象地址可能不稳定
        std::unordered_map<std::string,std::unique_ptr<Model>> model_cache_;
    };
} // renderer

#endif //PBRRENDERER_ASSETMANAGER_HPP