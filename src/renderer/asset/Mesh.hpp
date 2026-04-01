//
// Created by kaede on 2026/3/27.
//

#ifndef PBRRENDERER_MESH_HPP
#define PBRRENDERER_MESH_HPP
#include "../../core/noncopyable.hpp"
#include "../../core/opengl.hpp"
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "Vertex.hpp"

namespace renderer {

    class Mesh :public core::NonCopyable{
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void draw() const;

    private:
        void destroy() noexcept;

    private:
        GLuint vao_ = 0;
        unsigned int vbo_ = 0;
        unsigned int ebo_ = 0;

        int index_count_ = 0;
    };


}



#endif //PBRRENDERER_MESH_HPP