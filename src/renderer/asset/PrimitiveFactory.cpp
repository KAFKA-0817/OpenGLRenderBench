//
// Created by kaede on 2026/3/27.
//

#include "PrimitiveFactory.hpp"

namespace renderer {
    Model PrimitiveFactory::createCube() {
        std::vector<Vertex> vertices(24);

        // front (+z)
        vertices[0].position = {-0.5f, -0.5f,  0.5f};
        vertices[1].position = { 0.5f, -0.5f,  0.5f};
        vertices[2].position = { 0.5f,  0.5f,  0.5f};
        vertices[3].position = {-0.5f,  0.5f,  0.5f};
        for (int i = 0; i < 4; ++i) {
            vertices[i].normal = {0.0f, 0.0f, 1.0f};
        }
        vertices[0].texCoord = {0.0f, 0.0f};
        vertices[1].texCoord = {1.0f, 0.0f};
        vertices[2].texCoord = {1.0f, 1.0f};
        vertices[3].texCoord = {0.0f, 1.0f};

        // back (-z)
        vertices[4].position = { 0.5f, -0.5f, -0.5f};
        vertices[5].position = {-0.5f, -0.5f, -0.5f};
        vertices[6].position = {-0.5f,  0.5f, -0.5f};
        vertices[7].position = { 0.5f,  0.5f, -0.5f};
        for (int i = 4; i < 8; ++i) {
            vertices[i].normal = {0.0f, 0.0f, -1.0f};
        }
        vertices[4].texCoord = {0.0f, 0.0f};
        vertices[5].texCoord = {1.0f, 0.0f};
        vertices[6].texCoord = {1.0f, 1.0f};
        vertices[7].texCoord = {0.0f, 1.0f};

        // left (-x)
        vertices[8].position  = {-0.5f, -0.5f, -0.5f};
        vertices[9].position  = {-0.5f, -0.5f,  0.5f};
        vertices[10].position = {-0.5f,  0.5f,  0.5f};
        vertices[11].position = {-0.5f,  0.5f, -0.5f};
        for (int i = 8; i < 12; ++i) {
            vertices[i].normal = {-1.0f, 0.0f, 0.0f};
        }
        vertices[8].texCoord  = {0.0f, 0.0f};
        vertices[9].texCoord  = {1.0f, 0.0f};
        vertices[10].texCoord = {1.0f, 1.0f};
        vertices[11].texCoord = {0.0f, 1.0f};

        // right (+x)
        vertices[12].position = { 0.5f, -0.5f,  0.5f};
        vertices[13].position = { 0.5f, -0.5f, -0.5f};
        vertices[14].position = { 0.5f,  0.5f, -0.5f};
        vertices[15].position = { 0.5f,  0.5f,  0.5f};
        for (int i = 12; i < 16; ++i) {
            vertices[i].normal = {1.0f, 0.0f, 0.0f};
        }
        vertices[12].texCoord = {0.0f, 0.0f};
        vertices[13].texCoord = {1.0f, 0.0f};
        vertices[14].texCoord = {1.0f, 1.0f};
        vertices[15].texCoord = {0.0f, 1.0f};

        // bottom (-y)
        vertices[16].position = {-0.5f, -0.5f, -0.5f};
        vertices[17].position = { 0.5f, -0.5f, -0.5f};
        vertices[18].position = { 0.5f, -0.5f,  0.5f};
        vertices[19].position = {-0.5f, -0.5f,  0.5f};
        for (int i = 16; i < 20; ++i) {
            vertices[i].normal = {0.0f, -1.0f, 0.0f};
        }
        vertices[16].texCoord = {0.0f, 0.0f};
        vertices[17].texCoord = {1.0f, 0.0f};
        vertices[18].texCoord = {1.0f, 1.0f};
        vertices[19].texCoord = {0.0f, 1.0f};

        // top (+y)
        vertices[20].position = {-0.5f,  0.5f,  0.5f};
        vertices[21].position = { 0.5f,  0.5f,  0.5f};
        vertices[22].position = { 0.5f,  0.5f, -0.5f};
        vertices[23].position = {-0.5f,  0.5f, -0.5f};
        for (int i = 20; i < 24; ++i) {
            vertices[i].normal = {0.0f, 1.0f, 0.0f};
        }
        vertices[20].texCoord = {0.0f, 0.0f};
        vertices[21].texCoord = {1.0f, 0.0f};
        vertices[22].texCoord = {1.0f, 1.0f};
        vertices[23].texCoord = {0.0f, 1.0f};

        std::vector<unsigned int> indices = {
            0,1,2, 0,2,3,
            4,5,6, 4,6,7,
            8,9,10, 8,10,11,
            12,13,14, 12,14,15,
            16,17,18, 16,18,19,
            20,21,22, 20,22,23
        };

        Model model;
        model.add_mesh(Mesh(vertices, indices));
        return model;
    }

    Model PrimitiveFactory::createQuad() {
        std::vector<Vertex> vertices(4);

        vertices[0].position = {-1.0f, -1.0f, 0.0f};
        vertices[1].position = { 1.0f, -1.0f, 0.0f};
        vertices[2].position = { 1.0f,  1.0f, 0.0f};
        vertices[3].position = {-1.0f,  1.0f, 0.0f};


        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        Model model;
        model.add_mesh(Mesh(vertices, indices));
        return model;
    }

    Model PrimitiveFactory::createTriangle() {
        std::vector<Vertex> vertices(3);

        vertices[0].position = {-0.5f, -0.5f, 0.0f};
        vertices[1].position = { 0.5f, -0.5f, 0.0f};
        vertices[2].position = { 0.0f,  0.5f, 0.0f};

        std::vector<unsigned int> indices = {0, 1, 2};

        Model model;
        model.add_mesh(Mesh(vertices, indices));
        return model;
    }
}