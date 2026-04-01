//
// Created by kaede on 2026/3/31.
//

#ifndef PBRRENDERER_DEFERREDMATERIAL_HPP
#define PBRRENDERER_DEFERREDMATERIAL_HPP

#include "Material.hpp"

namespace renderer {
    class DeferredMaterial :public Material{
    public:
        DeferredMaterial()=default;
        ~DeferredMaterial() override = default;
        DeferredMaterial(DeferredMaterial&&)=delete;
        DeferredMaterial& operator=(DeferredMaterial&&)=delete;
    };
} // renderer

#endif //PBRRENDERER_DEFERREDMATERIAL_HPP