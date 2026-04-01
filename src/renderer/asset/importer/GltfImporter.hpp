//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_GLTFIMPORTER_HPP
#define PBRRENDERER_GLTFIMPORTER_HPP
#include "ImportedModelData.hpp"

namespace renderer {
    class GltfImporter {
    public:
        static ImportedModelData import_model_from_file(const std::filesystem::path& file_path);
    };
}



#endif //PBRRENDERER_GLTFIMPORTER_HPP