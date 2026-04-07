//
// Created by kaede on 2026/4/3.
//

#include "Scene.hpp"

#include <stdexcept>
#include <utility>

namespace editor {
    Entity Scene::createEntity() {
        entities_.emplace_back(next_entity_);

        if (next_entity_ >= entity_sparse_.size()) entity_sparse_.resize(next_entity_ + 1,-1);
        entity_sparse_[next_entity_] = entities_.size()-1;

        ++next_entity_;
        return entities_.back();
    }

    void Scene::destroyEntity(Entity entity) {
        if (!isAlive(entity)) {
            return;
        }

        auto dense_index = entity_sparse_[entity];
        auto back_entity = entities_.back();
        std::swap(entities_[dense_index],entities_.back());
        entities_.pop_back();
        entity_sparse_[entity]=-1;
        if (back_entity != entity) {
            entity_sparse_[back_entity]=dense_index;
        }

        nameComponents_.remove(entity);
        transformComponents_.remove(entity);
        meshRendererComponents_.remove(entity);
    }

    bool Scene::isAlive(Entity entity) const noexcept {
        if (entity >= entity_sparse_.size()) return false;

        const int dense_index = entity_sparse_[entity];
        if (dense_index < 0 || dense_index >= static_cast<int>(entities_.size())) {
            return false;
        }

        return entities_[dense_index] == entity;
    }

    NameComponent& Scene::addName(Entity entity, NameComponent component) {
        if (!isAlive(entity)) {
            throw std::runtime_error("Scene::addName on invalid entity");
        }

        return nameComponents_.insert(entity,std::move(component));
    }

    TransformComponent& Scene::addTransform(Entity entity, TransformComponent component) {
        if (!isAlive(entity)) {
            throw std::runtime_error("Scene::addTransform on invalid entity");
        }

        return transformComponents_.insert(entity,component);
    }

    MeshRendererComponent& Scene::addMeshRenderer(Entity entity, MeshRendererComponent component) {
        if (!isAlive(entity)) {
            throw std::runtime_error("Scene::addMeshRenderer on invalid entity");
        }

        return meshRendererComponents_.insert(entity,component);
    }

    bool Scene::hasName(Entity entity) const {
        return nameComponents_.contains(entity);
    }

    bool Scene::hasTransform(Entity entity) const {
        return transformComponents_.contains(entity);
    }

    bool Scene::hasMeshRenderer(Entity entity) const {
        return meshRendererComponents_.contains(entity);
    }

    NameComponent* Scene::tryGetName(Entity entity) {
        return nameComponents_.tryGet(entity);
    }

    TransformComponent* Scene::tryGetTransform(Entity entity) {
        return transformComponents_.tryGet(entity);
    }

    MeshRendererComponent* Scene::tryGetMeshRenderer(Entity entity) {
        return meshRendererComponents_.tryGet(entity);
    }

    const NameComponent* Scene::tryGetName(Entity entity) const {
        return nameComponents_.tryGet(entity);
    }

    const TransformComponent* Scene::tryGetTransform(Entity entity) const {
        return transformComponents_.tryGet(entity);
    }

    const MeshRendererComponent* Scene::tryGetMeshRenderer(Entity entity) const {
        return meshRendererComponents_.tryGet(entity);
    }
} // editor