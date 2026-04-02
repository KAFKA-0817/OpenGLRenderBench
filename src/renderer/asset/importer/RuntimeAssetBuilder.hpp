//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_RUNTIMEASSETBUILDER_HPP
#define PBRRENDERER_RUNTIMEASSETBUILDER_HPP
#include "ImportedModelData.hpp"
#include "../Model.hpp"

namespace renderer {
    class RuntimeAssetBuilder {
    public:
        static Model buildFromImported(const ImportedModelData& data);
    };
} // renderer

#endif //PBRRENDERER_RUNTIMEASSETBUILDER_HPP