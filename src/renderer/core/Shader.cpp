//
// Created by kaede on 2026/3/26.
//

#include "Shader.hpp"

#include <iostream>

#include "../../core/Log.hpp"
#include "../../core/file_io.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace renderer {
    Shader::Shader(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath)
        :vertex_path_(vertexShaderPath),fragment_path_(fragmentShaderPath)
    {
        id_ = buildProgramFromFiles(vertexShaderPath, fragmentShaderPath);
    }

    Shader::~Shader() {
        if (id_ != 0) {
            glDeleteProgram(id_);
        }
    }

    Shader::Shader(Shader&& other) noexcept
    :id_(other.id_),vertex_path_(std::move(other.vertex_path_)),fragment_path_(std::move(other.fragment_path_))
    {
        other.id_ = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (id_ != 0) {
                glDeleteProgram(id_);
            }
            id_ = other.id_;
            vertex_path_ = std::move(other.vertex_path_);
            fragment_path_ = std::move(other.fragment_path_);
            other.id_ = 0;
        }
        return *this;
    }

    void Shader::use() const {
        glUseProgram(id_);
    }

    GLuint Shader::compileShader(const std::string &source, GLenum type) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            int log_len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

            std::string log(log_len, '\0');
            glGetShaderInfoLog(shader, log_len, nullptr, log.data());

            glDeleteShader(shader);

            throw std::runtime_error("[Shader] Compile error:\n" + log);
        }

        return shader;
    }

    GLuint Shader::linkProgram(GLuint vs, GLuint fs) {
        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success) {
            int log_len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

            std::string log(log_len, '\0');
            glGetProgramInfoLog(program, log_len, nullptr, log.data());

            glDeleteProgram(program);

            throw std::runtime_error("[Shader] Link error:\n" + log);
        }

        return program;
    }

    GLuint Shader::buildProgramFromFiles(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path) {
        auto vs_source = core::io::read_text_file(vertex_path);
        auto fs_source = core::io::read_text_file(fragment_path);
        GLuint vs = compileShader(vs_source, GL_VERTEX_SHADER);
        GLuint fs = compileShader(fs_source, GL_FRAGMENT_SHADER);

        GLuint id = linkProgram(vs, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return id;
    }

    bool Shader::reload() {
        try {
            GLuint new_program = buildProgramFromFiles(vertex_path_, fragment_path_);

            if (id_ != 0) {
                glDeleteProgram(id_);
            }
            id_ = new_program;

            core::Log::getInstance().write(
                "Shader",
                "Reload succeeded: " + vertex_path_.string() + " | " + fragment_path_.string()
            );
            return true;
        } catch (const std::exception& e) {
            core::Log::getInstance().write(
                "Shader",
                "Reload failed: " + std::string(e.what())
            );
            return false;
        }
    }

    void Shader::setInt(const std::string& name, int value) const{
        glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const{
        glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& vec) const{
        glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, glm::value_ptr(vec));
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& mat) const{
        glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
}
