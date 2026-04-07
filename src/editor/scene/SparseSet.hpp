//
// Created by kaede on 2026/4/6.
//

#ifndef PBRRENDERER_SPARSESET_HPP
#define PBRRENDERER_SPARSESET_HPP
#include <vector>

#include "Entity.hpp"
#include "../../core/noncopyable.hpp"

namespace editor {

    template <class T>
    class SparseSet : public core::NonCopyable{
    public:
        SparseSet() = default;
        ~SparseSet() = default;
        SparseSet(SparseSet&& other) noexcept = default;
        SparseSet& operator=(SparseSet&& other) noexcept = default;

        bool contains(Entity entity) const noexcept;
        T& get(Entity entity);
        const T& get(Entity entity) const;
        T* tryGet(Entity entity);
        const T* tryGet(Entity entity) const;
        T& insert(Entity entity, T component);
        void remove(Entity entity);
        void clear();
        std::size_t size() const { return entity_dense_.size();}

        const std::vector<Entity>& getEntities() const { return entity_dense_; }
        const std::vector<T>& getComponents() const { return component_dense_; }


    private:
        std::vector<Entity> entity_dense_;
        std::vector<int> entity_sparse_;
        std::vector<T> component_dense_;
    };

        template<class T>
    bool SparseSet<T>::contains(Entity entity) const noexcept {
        if (entity>=entity_sparse_.size()) return false;

        auto index = entity_sparse_[entity];
        if (index==-1 || index >= entity_dense_.size()) return false;
        return entity_dense_[index]==entity;
    }

    template<class T>
    T& SparseSet<T>::get(Entity entity) {
        auto index = entity_sparse_[entity];
        return component_dense_[index];
    }

    template<class T>
    const T& SparseSet<T>::get(Entity entity) const {
        auto index = entity_sparse_[entity];
        return component_dense_[index];
    }

    template<class T>
    T* SparseSet<T>::tryGet(Entity entity) {
        if (!contains(entity)) return nullptr;
        return &get(entity);
    }

    template<class T>
    const T* SparseSet<T>::tryGet(Entity entity) const {
        if (!contains(entity)) return nullptr;
        return &get(entity);
    }

    template<class T>
    void SparseSet<T>::clear() {
        entity_dense_.clear();
        component_dense_.clear();
        entity_sparse_.clear();
    }

    template<class T>
    T& SparseSet<T>::insert(Entity entity, T component) {
        if (contains(entity)) {
            auto dense_index = entity_sparse_[entity];
            component_dense_[dense_index]=std::move(component);
            return component_dense_[dense_index];
        }

        if (entity>=entity_sparse_.size()) {
            entity_sparse_.resize(entity+1,-1);
        }

        entity_dense_.push_back(entity);
        component_dense_.push_back(std::move(component));
        auto index = entity_dense_.size() - 1;
        entity_sparse_[entity] = index;
        return component_dense_[index];
    }

    template<class T>
    void SparseSet<T>::remove(Entity entity) {
        if (!contains(entity)) return;

        auto dense_index = entity_sparse_[entity];
        auto back_entity = entity_dense_.back();
        std::swap(entity_dense_[dense_index],entity_dense_.back());
        entity_dense_.pop_back();
        std::swap(component_dense_[dense_index],component_dense_.back());
        component_dense_.pop_back();

        entity_sparse_[entity]=-1;
        if (back_entity!=entity)
            entity_sparse_[back_entity]=dense_index;
    }
} // editor

#endif //PBRRENDERER_SPARSESET_HPP
