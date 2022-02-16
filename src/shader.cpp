/*
 * spnda, Copyright (c) 2020
 * shader.cpp
 */

#include <fstream>
#include <iostream>
#include <sstream>

#include <shader.h>

std::string Program::readFile(fs::path path) {
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream shaderStream;
        shaderStream << file.rdbuf();
        file.close();
        code = shaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "Failed to read shader file: " << e.what() << std::endl;
    }
    return std::move(code);
}

GLuint Program::compileShader(std::string& code, GLenum type) {
    const char* codeC = code.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &codeC, nullptr);
    glCompileShader(shader);
    return shader;
}

GLuint Program::createProgram(std::initializer_list<GLuint> shaders) {
    GLuint id = glCreateProgram();
    for (auto& shader : shaders)
        glAttachShader(id, shader);
    glLinkProgram(id);
    checkCompileErrors(id, Program::ProgramType::Program);

    for (auto& shader : shaders)
        glDeleteShader(shader);
    return id;
}

void Program::checkCompileErrors(unsigned int shader, ProgramType type) {
    int success;
    const int length = 1024;
    char infoLog[length];
    if (type != ProgramType::Program) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, length, nullptr, infoLog);
            std::cout << "Shader compilation error of type: " << static_cast<uint32_t>(type) << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shader, length, nullptr, infoLog);
            std::cout << "Shader linking error of type: " << static_cast<uint32_t>(type) << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void Program::setName(fs::path path) { name = path.filename().replace_extension("").string(); }

void ComputeProgram::create(fs::path path) {
    if (!fs::is_regular_file(path)) {
        return;
    }

    std::string shaderCode = readFile(path);
    const char* shaderCodeC = shaderCode.c_str();

    id = glCreateShaderProgramv(GL_COMPUTE_SHADER, 1, &shaderCodeC);
    checkCompileErrors(id, Program::ProgramType::Program);

    setName(path);
}

void RasterizedProgram::create(fs::path vertPath, fs::path fragPath) {
    if (!fs::is_regular_file(vertPath) || !fs::is_regular_file(fragPath)) {
        return;
    }

    std::string fragCode = readFile(fragPath), vertCode = readFile(vertPath);
    std::ifstream fragFile, vertFile;

    GLuint fragment = compileShader(fragCode, GL_FRAGMENT_SHADER);
    GLuint vertex = compileShader(vertCode, GL_VERTEX_SHADER);

    checkCompileErrors(fragment, ProgramType::Fragment);
    checkCompileErrors(vertex, ProgramType::Vertex);

    id = createProgram({ fragment, vertex });

    setName(vertPath);
}
