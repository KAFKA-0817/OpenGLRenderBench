//
// Created by kaede on 2026/3/31.
//

#include "GltfImporter.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tiny_gltf.h"

namespace renderer {

    namespace {
        glm::mat4 nodeLocalTransform(const tinygltf::Node& node) {
            glm::mat4 result;

            if (node.matrix.size() == 16) {
                result = glm::make_mat4(node.matrix.data());
                return result;
            }

            glm::vec3 translation(0.0f);
            if (node.translation.size() == 3) {
                translation = glm::vec3(
                    static_cast<float>(node.translation[0]),
                    static_cast<float>(node.translation[1]),
                    static_cast<float>(node.translation[2]));
            }

            glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
            if (node.rotation.size() == 4) {
                rotation = glm::quat(
                    static_cast<float>(node.rotation[3]),
                    static_cast<float>(node.rotation[0]),
                    static_cast<float>(node.rotation[1]),
                    static_cast<float>(node.rotation[2]));
            }

            glm::vec3 scale(1.0f);
            if (node.scale.size() == 3) {
                scale = glm::vec3(
                    static_cast<float>(node.scale[0]),
                    static_cast<float>(node.scale[1]),
                    static_cast<float>(node.scale[2]));
            }

            result = glm::translate(glm::mat4(1.0f), translation)
                   * glm::mat4_cast(rotation)
                   * glm::scale(glm::mat4(1.0f), scale);

            return result;
        }

        int componentCount(int type) {
            switch (type) {
                case TINYGLTF_TYPE_SCALAR: return 1;
                case TINYGLTF_TYPE_VEC2:   return 2;
                case TINYGLTF_TYPE_VEC3:   return 3;
                case TINYGLTF_TYPE_VEC4:   return 4;
                default: return 0;
            }
        }

        int componentSize(int component_type) {
            switch (component_type) {
                case TINYGLTF_COMPONENT_TYPE_BYTE:
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    return 1;
                case TINYGLTF_COMPONENT_TYPE_SHORT:
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    return 2;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                case TINYGLTF_COMPONENT_TYPE_FLOAT:
                    return 4;
                default:
                    return 0;
            }
        }

        size_t accessorStride(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
            const tinygltf::BufferView& view = model.bufferViews.at(accessor.bufferView);
            size_t stride = accessor.ByteStride(view);
            if (stride == 0) {
                stride = static_cast<size_t>(componentCount(accessor.type) * componentSize(accessor.componentType));
            }
            return stride;
        }

        const unsigned char* accessorDataPtr(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
            const tinygltf::BufferView& view = model.bufferViews.at(accessor.bufferView);
            const tinygltf::Buffer& buffer = model.buffers.at(view.buffer);

            size_t offset = static_cast<size_t>(view.byteOffset) + static_cast<size_t>(accessor.byteOffset);
            return buffer.data.data() + offset;
        }

        glm::vec2 readVec2At(const tinygltf::Model& model, const tinygltf::Accessor& accessor, size_t index) {
            const unsigned char* base = accessorDataPtr(model, accessor);
            size_t stride = accessorStride(model, accessor);
            const float* ptr = reinterpret_cast<const float*>(base + index * stride);
            return glm::vec2(ptr[0], ptr[1]);
        }

        glm::vec3 readVec3At(const tinygltf::Model& model, const tinygltf::Accessor& accessor, size_t index) {
            const unsigned char* base = accessorDataPtr(model, accessor);
            size_t stride = accessorStride(model, accessor);
            const float* ptr = reinterpret_cast<const float*>(base + index * stride);
            return glm::vec3(ptr[0], ptr[1], ptr[2]);
        }

        glm::vec4 readVec4At(const tinygltf::Model& model, const tinygltf::Accessor& accessor, size_t index) {
            const unsigned char* base = accessorDataPtr(model, accessor);
            size_t stride = accessorStride(model, accessor);
            const float* ptr = reinterpret_cast<const float*>(base + index * stride);
            return glm::vec4(ptr[0], ptr[1], ptr[2], ptr[3]);
        }

        unsigned int readIndexAt(const tinygltf::Model& model, const tinygltf::Accessor& accessor, size_t index) {
            const unsigned char* base = accessorDataPtr(model, accessor);
            size_t stride = accessorStride(model, accessor);
            const unsigned char* ptr = base + index * stride;

            switch (accessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    return *reinterpret_cast<const uint8_t*>(ptr);
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    return *reinterpret_cast<const uint16_t*>(ptr);
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    return *reinterpret_cast<const uint32_t*>(ptr);
                default:
                    throw std::runtime_error("Unsupported index component type.");
            }
        }


        AlphaMode parseAlphaMode(const std::string& value) {
            if (value == "MASK") return AlphaMode::Mask;
            if (value == "BLEND") return AlphaMode::Blend;
            return AlphaMode::Opaque;
        }

        int resolveImageIndex(const tinygltf::Model& model, int texture_index) {
            if (texture_index < 0) return -1;
            const tinygltf::Texture& texture = model.textures.at(texture_index);
            return texture.source >= 0 ? texture.source : -1;
        }

        void importMaterials(const tinygltf::Model& model, ImportedModelData& imported) {
            imported.materials.reserve(model.materials.size());

            for (const auto& src_mat:model.materials) {
                PBRMaterialData mat;
                mat.name = src_mat.name;
                const auto& pbr = src_mat.pbrMetallicRoughness;

                if (pbr.baseColorFactor.size() == 4) {
                    mat.base_color_factor = glm::vec4(
                        static_cast<float>(pbr.baseColorFactor[0]),
                        static_cast<float>(pbr.baseColorFactor[1]),
                        static_cast<float>(pbr.baseColorFactor[2]),
                        static_cast<float>(pbr.baseColorFactor[3]));
                }
                mat.metallic_factor = static_cast<float>(pbr.metallicFactor);
                mat.roughness_factor = static_cast<float>(pbr.roughnessFactor);
                mat.base_color_image = resolveImageIndex(model, pbr.baseColorTexture.index);
                mat.metallic_roughness_image = resolveImageIndex(model, pbr.metallicRoughnessTexture.index);
                mat.normal_image = resolveImageIndex(model, src_mat.normalTexture.index);
                mat.occlusion_image = resolveImageIndex(model, src_mat.occlusionTexture.index);
                mat.emissive_image = resolveImageIndex(model, src_mat.emissiveTexture.index);

                mat.normal_scale = static_cast<float>(src_mat.normalTexture.scale);
                mat.occlusion_strength = static_cast<float>(src_mat.occlusionTexture.strength);
                if (src_mat.emissiveFactor.size() == 3) {
                    mat.emissive_factor = glm::vec3(
                        static_cast<float>(src_mat.emissiveFactor[0]),
                        static_cast<float>(src_mat.emissiveFactor[1]),
                        static_cast<float>(src_mat.emissiveFactor[2]));
                }

                mat.alpha_mode = parseAlphaMode(src_mat.alphaMode);
                mat.alpha_cutoff = static_cast<float>(src_mat.alphaCutoff);
                mat.double_sided = src_mat.doubleSided;
                imported.materials.push_back(std::move(mat));
            }
        }

        void importImages(const tinygltf::Model& model, ImportedModelData& imported) {
            imported.images.reserve(model.images.size());

            for (size_t i = 0; i < model.images.size(); ++i) {
                const auto& src_img = model.images[i];

                ImageData img;
                img.name = src_img.name;
                img.width = src_img.width;
                img.height = src_img.height;
                img.channels = src_img.component;
                img.srgb = false;
                img.pixels = src_img.image;

                imported.images.push_back(std::move(img));
            }

            for (const auto& material : imported.materials) {
                if (material.base_color_image >= 0) {
                    imported.images[material.base_color_image].srgb = true;
                }
                if (material.emissive_image >= 0) {
                    imported.images[material.emissive_image].srgb = true;
                }
            }
        }

        void importPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const glm::mat4& accumulated_transform, ImportedModelData& imported) {
            if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
                throw std::runtime_error("Only triangle primitives are supported.");
            }
            auto pos_it = primitive.attributes.find("POSITION");
            auto nor_it = primitive.attributes.find("NORMAL");
            auto uv_it  = primitive.attributes.find("TEXCOORD_0");
            auto tan_it = primitive.attributes.find("TANGENT");

            if (pos_it == primitive.attributes.end()) {
                throw std::runtime_error("Primitive missing POSITION.");
            }
            if (nor_it == primitive.attributes.end()) {
                throw std::runtime_error("Primitive missing NORMAL.");
            }
            if (uv_it == primitive.attributes.end()) {
                throw std::runtime_error("Primitive missing TEXCOORD_0.");
            }
            if (primitive.indices < 0) {
                throw std::runtime_error("Primitive missing indices.");
            }
            const tinygltf::Accessor& pos_accessor = model.accessors.at(pos_it->second);
            const tinygltf::Accessor& nor_accessor = model.accessors.at(nor_it->second);
            const tinygltf::Accessor& uv_accessor  = model.accessors.at(uv_it->second);
            const tinygltf::Accessor* tan_accessor = nullptr;
            if (tan_it != primitive.attributes.end()) {
                tan_accessor = &model.accessors.at(tan_it->second);
            }
            SubmeshData submesh;
            submesh.material_index = primitive.material;

            const size_t vertex_count = pos_accessor.count;
            submesh.vertices.resize(vertex_count);
            glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(accumulated_transform)));
            for (size_t v = 0; v < vertex_count; ++v) {
                Vertex vert{};

                glm::vec3 position = readVec3At(model, pos_accessor, v);
                glm::vec3 normal   = readVec3At(model, nor_accessor, v);
                glm::vec2 uv       = readVec2At(model, uv_accessor, v);

                glm::vec4 tangent4(0.0f);
                bool has_tangent = tan_accessor != nullptr;
                if (has_tangent) {
                    tangent4 = readVec4At(model, *tan_accessor, v);
                }

                glm::vec4 world_position = accumulated_transform * glm::vec4(position, 1.0f);
                glm::vec3 world_normal = glm::normalize(normal_matrix * normal);

                vert.position = glm::vec3(world_position);
                vert.normal = world_normal;
                vert.texCoord = uv;

                if (has_tangent) {
                    glm::vec3 world_tangent = glm::normalize(normal_matrix * glm::vec3(tangent4));
                    glm::vec3 world_bitangent = glm::normalize(glm::cross(world_normal, world_tangent) * tangent4.w);
                    vert.tangent = world_tangent;
                    vert.bitangent = world_bitangent;
                }

                submesh.vertices[v] = vert;
            }

            const tinygltf::Accessor& index_accessor = model.accessors.at(primitive.indices);
            submesh.indices.resize(index_accessor.count);
            for (size_t i = 0; i < index_accessor.count; ++i) {
                submesh.indices[i] = readIndexAt(model, index_accessor, i);
            }

            imported.submeshes.push_back(std::move(submesh));

        }

        void importNodeRecursive(const tinygltf::Model& model, int node_index, const glm::mat4& parent_transform, ImportedModelData& imported) {
            const tinygltf::Node& node = model.nodes.at(node_index);
            glm::mat4 local = nodeLocalTransform(node);
            glm::mat4 accumulated = parent_transform * local;

            if (node.mesh >= 0) {
                const tinygltf::Mesh& mesh = model.meshes.at(node.mesh);
                for (const auto& primitive : mesh.primitives) {
                    importPrimitive(model, primitive, accumulated, imported);
                }
            }

            for (int child_index : node.children) {
                importNodeRecursive(model, child_index, accumulated, imported);
            }
        }

    }

    ImportedModelData GltfImporter::import_model_from_file(const std::filesystem::path &file_path) {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        std::string error, warning;
        bool ok = false;
        if (file_path.extension() == ".glb") {
            ok = loader.LoadBinaryFromFile(&model, &error, &warning, file_path.string());
        } else {
            ok = loader.LoadASCIIFromFile(&model, &error, &warning, file_path.string());
        }
        if (!ok) {
            throw std::runtime_error("Failed to load glTF: " + error);
        }

        ImportedModelData imported;
        imported.name = file_path.stem().string();
        imported.path = file_path;

        importMaterials(model, imported);
        importImages(model, imported);

        int scene_index = model.defaultScene >= 0 ? model.defaultScene : 0;
        if (scene_index < 0 || scene_index >= static_cast<int>(model.scenes.size())) {
            throw std::runtime_error("glTF has no valid default scene.");
        }

        const tinygltf::Scene& scene = model.scenes.at(scene_index);
        for (int node_index : scene.nodes) {
            importNodeRecursive(model, node_index, glm::mat4(1.0f), imported);
        }

        return imported;
    }
}
