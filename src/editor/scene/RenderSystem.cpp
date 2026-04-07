//
// Created by kaede on 2026/4/3.
//

#include "RenderSystem.hpp"

#include "../../renderer/pipeline/Renderer.hpp"

namespace editor {
    void RenderSystem::renderScene(renderer::Renderer& renderer, Scene& scene) {
        for (const auto& entity:scene.entities()) {
            auto meshComponent = scene.tryGetMeshRenderer(entity);
            auto transformComponent = scene.tryGetTransform(entity);
            if (!meshComponent || !transformComponent)  continue;
            if (!meshComponent->visible)  continue;

            auto modelMatrix = transformComponent->modelMatrix();
            auto model = meshComponent->model;
            if (!model) return;
            for (const auto& mesh:model->meshes()) {
                if (mesh.material_index < 0 || mesh.material_index >= static_cast<int>(model->materials().size()))  continue;
                renderer.submit(mesh.mesh,*model->materials()[mesh.material_index],modelMatrix);
            }
        }
    }
}
