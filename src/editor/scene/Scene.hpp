//
// Created by kaede on 2026/4/3.
//

#ifndef PBRRENDERER_SCENE_HPP
#define PBRRENDERER_SCENE_HPP
#include <unordered_map>
#include <vector>

#include "Components.hpp"
#include "Entity.hpp"
#include "../../core/noncopyable.hpp"

namespace editor {
    class Scene :public core::NonCopyable{
    public:
        Scene() = default;
        ~Scene() = default;
        Scene(Scene&&) noexcept = delete;
        Scene& operator=(Scene&&) noexcept = delete;

        const std::vector<Entity>& entities() const noexcept { return entities_; }
        Entity createEntity();
        void destroyEntity(Entity entity);
        bool isAlive(Entity entity) const noexcept;

        NameComponent& addName(Entity entity, NameComponent component = {});
        TransformComponent& addTransform(Entity entity, TransformComponent component = {});
        MeshRendererComponent& addMeshRenderer(Entity entity, MeshRendererComponent component = {});

        bool hasName(Entity entity) const;
        bool hasTransform(Entity entity) const;
        bool hasMeshRenderer(Entity entity) const;

        NameComponent* tryGetName(Entity entity);
        TransformComponent* tryGetTransform(Entity entity);
        MeshRendererComponent* tryGetMeshRenderer(Entity entity);

        const NameComponent* tryGetName(Entity entity) const;
        const TransformComponent* tryGetTransform(Entity entity) const;
        const MeshRendererComponent* tryGetMeshRenderer(Entity entity) const;

    private:
        Entity next_entity_ = -1;
        std::vector<Entity> entities_;

        std::unordered_map<Entity,NameComponent> names_;
        std::unordered_map<Entity,TransformComponent> transforms_;
        std::unordered_map<Entity,MeshRendererComponent> meshes_;
    };
} // editor

#endif //PBRRENDERER_SCENE_HPP