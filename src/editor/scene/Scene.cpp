//
// Created by kaede on 2026/4/3.
//

#include "Scene.hpp"

#include <stdexcept>

namespace editor {
    Entity Scene::createEntity() {
        ++next_entity_;
        entities_.emplace_back(next_entity_);
        return entities_.back();
    }

    void Scene::destroyEntity(Entity entity) {
        entities_.erase(
            std::remove(entities_.begin(), entities_.end(), entity),
            entities_.end()
            );

        names_.erase(entity);
        transforms_.erase(entity);
        meshes_.erase(entity);
    }

    bool Scene::isAlive(Entity entity) const noexcept {
        return std::find(entities_.begin(), entities_.end(), entity) != entities_.end();
    }

    NameComponent& Scene::addName(Entity entity, NameComponent component) {
        if (!isAlive(entity)) {
            throw std::runtime_error("Scene::addName on invalid entity");
        }

        auto pair = names_.insert_or_assign(entity,std::move(component));
        return pair.first->second;
    }

    TransformComponent& Scene::addTransform(Entity entity, TransformComponent component) {
        if (!isAlive(entity)) {
            throw std::runtime_error("Scene::addTransform on invalid entity");
        }

        auto pair = transforms_.insert_or_assign(entity,component);
        return pair.first->second;
    }

    MeshRendererComponent& Scene::addMeshRenderer(Entity entity, MeshRendererComponent component) {
        if (!isAlive(entity)) {
            throw std::runtime_error("Scene::addMeshRenderer on invalid entity");
        }
        auto pair = meshes_.insert_or_assign(entity,component);
        return pair.first->second;
    }

    bool Scene::hasName(Entity entity) const {
        return names_.find(entity) != names_.end();
    }

    bool Scene::hasTransform(Entity entity) const {
        return transforms_.find(entity) != transforms_.end();
    }

    bool Scene::hasMeshRenderer(Entity entity) const {
        return meshes_.find(entity) != meshes_.end();
    }

    NameComponent* Scene::tryGetName(Entity entity) {
        return names_.find(entity) != names_.end() ? &names_.at(entity) : nullptr;
    }

    TransformComponent* Scene::tryGetTransform(Entity entity) {
        return transforms_.find(entity) != transforms_.end() ? &transforms_.at(entity) : nullptr;
    }

    MeshRendererComponent* Scene::tryGetMeshRenderer(Entity entity) {
        return meshes_.find(entity) != meshes_.end() ? &meshes_.at(entity) : nullptr;
    }

    const NameComponent* Scene::tryGetName(Entity entity) const {
        return names_.find(entity) != names_.end() ? &names_.at(entity) : nullptr;
    }

    const TransformComponent* Scene::tryGetTransform(Entity entity) const {
        return transforms_.find(entity) != transforms_.end() ? &transforms_.at(entity) : nullptr;
    }

    const MeshRendererComponent* Scene::tryGetMeshRenderer(Entity entity) const {
        return meshes_.find(entity) != meshes_.end() ? &meshes_.at(entity) : nullptr;
    }
} // editor