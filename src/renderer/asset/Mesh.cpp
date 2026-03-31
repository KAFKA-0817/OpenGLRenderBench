//
// Created by kaede on 2026/3/27.
//

#include "Mesh.hpp"

namespace renderer {
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int> &indices) {
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);

        index_count_ = static_cast<int>(indices.size());
    }

    Mesh::~Mesh() {
        destroy();
    }

    void Mesh::destroy() noexcept {
        if (vao_ != 0) {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0;
        }
        if (vbo_ != 0) {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }
        if (ebo_ != 0) {
            glDeleteBuffers(1, &ebo_);
            ebo_ = 0;
        }
        index_count_ = 0;
    }

    Mesh::Mesh(Mesh&& other) noexcept {
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        index_count_ = other.index_count_;
        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
        other.index_count_ = 0;
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept {
        if (&other != this) {
            destroy();
            vao_ = other.vao_;
            vbo_ = other.vbo_;
            ebo_ = other.ebo_;
            index_count_ = other.index_count_;

            other.vao_ = 0;
            other.vbo_ = 0;
            other.ebo_ = 0;
            other.index_count_ = 0;
        }

        return *this;
    }

    void Mesh::draw() const {
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}
