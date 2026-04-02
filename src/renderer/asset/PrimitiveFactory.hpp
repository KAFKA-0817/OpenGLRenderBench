//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_PRIMITIVEFACTORY_HPP
#define PBRRENDERER_PRIMITIVEFACTORY_HPP
#include "Model.hpp"

namespace renderer {
    class PrimitiveFactory {
    public:
        static Mesh createCube();
        static Mesh createQuad();
        static Mesh createTriangle();
    };
}


#endif //PBRRENDERER_PRIMITIVEFACTORY_HPP