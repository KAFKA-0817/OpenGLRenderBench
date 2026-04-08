//
// Created by kaede on 2026/4/3.
//

#ifndef PBRRENDERER_SCENE_HPP
#define PBRRENDERER_SCENE_HPP
#include <vector>

#include "Components.hpp"
#include "Entity.hpp"
#include "SparseSet.hpp"
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
        DirectionalLightComponent& addDirectionalLight(Entity entity, DirectionalLightComponent component = {});
        PointLightComponent& addPointLight(Entity entity, PointLightComponent component = {});
        void removeName(Entity entity) { nameComponents_.remove(entity); }
        void removeTransform(Entity entity) { transformComponents_.remove(entity); }
        void removeMeshRenderer(Entity entity) { meshRendererComponents_.remove(entity); }
        void removeDirectionalLight(Entity entity) { directionalLightComponents_.remove(entity); }
        void removePointLight(Entity entity) { pointLightComponents_.remove(entity); }

        bool hasName(Entity entity) const;
        bool hasTransform(Entity entity) const;
        bool hasMeshRenderer(Entity entity) const;
        bool hasDirectionalLight(Entity entity) const;
        bool hasPointLight(Entity entity) const;

        NameComponent* tryGetName(Entity entity);
        TransformComponent* tryGetTransform(Entity entity);
        MeshRendererComponent* tryGetMeshRenderer(Entity entity);
        DirectionalLightComponent* tryGetDirectionalLight(Entity entity);
        PointLightComponent* tryGetPointLight(Entity entity);

        const NameComponent* tryGetName(Entity entity) const;
        const TransformComponent* tryGetTransform(Entity entity) const;
        const MeshRendererComponent* tryGetMeshRenderer(Entity entity) const;
        const DirectionalLightComponent* tryGetDirectionalLight(Entity entity) const;
        const PointLightComponent* tryGetPointLight(Entity entity) const;

    private:
        Entity next_entity_ = 1;
        std::vector<Entity> entities_;
        std::vector<int> entity_sparse_;

        SparseSet<NameComponent> nameComponents_;
        SparseSet<TransformComponent> transformComponents_;
        SparseSet<MeshRendererComponent> meshRendererComponents_;
        SparseSet<DirectionalLightComponent> directionalLightComponents_;
        SparseSet<PointLightComponent> pointLightComponents_;
    };
} // editor

#endif //PBRRENDERER_SCENE_HPP
