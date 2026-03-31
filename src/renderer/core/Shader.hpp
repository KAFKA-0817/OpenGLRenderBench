//
// Created by kaede on 2026/3/26.
//

#ifndef PBRRENDERER_SHADER_HPP
#define PBRRENDERER_SHADER_HPP
#include "../../core/opengl.hpp"
#include <filesystem>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include "../../core/noncopyable.hpp"

namespace renderer {
    class Shader : public core::NonCopyable{
    public:
        Shader(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);
        ~Shader();
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;

        void use() const;
        bool reload();

        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setMat4(const std::string& name, const glm::mat4& value) const;

    private:
        static GLuint compileShader(const std::string& source, GLenum type);
        static GLuint linkProgram(GLuint vs, GLuint fs);
        static GLuint buildProgramFromFiles(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path);

    private:
        GLuint id_;
        std::filesystem::path vertex_path_;
        std::filesystem::path fragment_path_;
    };
}



#endif //PBRRENDERER_SHADER_HPP