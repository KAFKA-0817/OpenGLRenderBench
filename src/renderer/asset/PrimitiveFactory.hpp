//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_PRIMITIVEFACTORY_HPP
#define PBRRENDERER_PRIMITIVEFACTORY_HPP
#include "Model.hpp"

namespace renderer {
    class PrimitiveFactory {
    public:
        static Model createCube();
        static Model createQuad();
        static Model createTriangle();
    };
}


#endif //PBRRENDERER_PRIMITIVEFACTORY_HPP