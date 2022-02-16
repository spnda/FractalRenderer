/*
 * spnda, Copyright (c) 2020
 * shader.h
 */

#pragma once

#include <filesystem>
#include <string>

#include <glad/glad.h>

#include <vector2.h>
#include <vector3.h>

namespace fs = std::filesystem;

class Program {
protected:
    enum class ProgramType {
        Program,
        Vertex,
        Fragment,
    };

    GLuint id;

    explicit Program() = default;
    ~Program() = default;

    [[nodiscard]] std::string readFile(fs::path path);
    [[nodiscard]] GLuint compileShader(std::string& code, GLenum type);
    [[nodiscard]] GLuint createProgram(std::initializer_list<GLuint> shaders);
    void checkCompileErrors(unsigned int shader, ProgramType type);
    void setName(fs::path filePath);

public:
    std::string name;

    void use() { glUseProgram(id); }

    void disable() { glUseProgram(0); }

    void setBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value); }

    void setInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(id, name.c_str()), value); }

    void setFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(id, name.c_str()), value); }

    void setDouble(const std::string& name, double value) const { glUniform1f(glGetUniformLocation(id, name.c_str()), float(value)); }

    void setVec2(const std::string& name, float x, float y) const {
        float vec2[2] = { x, y };
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, vec2);
    }

    void setVec2(const std::string& name, uint32_t x, uint32_t y) const { glUniform2i(glGetUniformLocation(id, name.c_str()), x, y); }

    void setVec3(const std::string& name, vector3 value) const {
        float vec3[3] = { value.x, value.y, value.z };
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, vec3);
    }
};

class ComputeProgram final : public Program {
public:
    explicit ComputeProgram() = default;

    void create(fs::path path);
};

// Represents a program used with a rasterization pipeline.
// Commonly includes a vertex and fragment shader.
class RasterizedProgram final : public Program {
public:
    explicit RasterizedProgram() = default;

    void create(fs::path vertPath, fs::path fragPath);
};
