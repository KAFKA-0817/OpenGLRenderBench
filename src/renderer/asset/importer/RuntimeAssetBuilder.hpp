//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_RUNTIMEASSETBUILDER_HPP
#define PBRRENDERER_RUNTIMEASSETBUILDER_HPP
#include "ImportedModelData.hpp"
#include "RuntimeModelAsset.hpp"

namespace renderer {
    class RuntimeAssetBuilder {
    public:
        static RuntimeModelAsset buildFromImported(const ImportedModelData& data);
    };
} // renderer

#endif //PBRRENDERER_RUNTIMEASSETBUILDER_HPP