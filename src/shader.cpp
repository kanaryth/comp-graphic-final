#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <OpenGL/gl3.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(
    const std::string& vertexPath,
    const std::string& fragmentPath
)
{
    ID = 0;

    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vertexFile(vertexPath);
    std::ifstream fragmentFile(fragmentPath);

    if (!vertexFile.is_open() || !fragmentFile.is_open()) {
        std::cerr << "ERROR: Failed to open shader files\n";
        return;
    }

    std::stringstream vertexStream;
    std::stringstream fragmentStream;

    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    vertexCode = vertexStream.str();
    fragmentCode = fragmentStream.str();

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);

    int success;
    char infoLog[1024];

    glShaderSource(vertex, 1, &vertexSource, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
        std::cerr << "Vertex shader error:\n" << infoLog << '\n';
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return;
    }

    glShaderSource(fragment, 1, &fragmentSource, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragment, 1024, nullptr, infoLog);
        std::cerr << "Fragment shader error:\n" << infoLog << '\n';
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return;
    }

    ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << '\n';

        glDeleteProgram(ID);
        ID = 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec3(
    const std::string& name,
    const glm::vec3& value
) const
{
    glUniform3fv(
        glGetUniformLocation(ID, name.c_str()),
        1,
        glm::value_ptr(value)
    );
}

void Shader::SetMat4(
    const std::string& name,
    const glm::mat4& value
) const
{
    glUniformMatrix4fv(
        glGetUniformLocation(ID, name.c_str()),
        1,
        GL_FALSE,
        glm::value_ptr(value)
    );
}