
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <OpenGL/gl3.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vertexFile(vertexPath);
    std::ifstream fragmentFile(fragmentPath);

    if (!vertexFile.is_open() || !fragmentFile.is_open())
    {
        std::cerr << "ERROR: Failed to open shader files\n";
        ID = 0;
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
    glShaderSource(vertex, 1, &vertexSource, nullptr);
    glCompileShader(vertex);

    int success;
    char infoLog[512];

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Error:\n" << infoLog << '\n';
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Error:\n" << infoLog << '\n';
    }

    ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Shader Linking Error:\n" << infoLog << '\n';
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
    SetInt(name, static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(
        glGetUniformLocation(ID, name.c_str()),
        1,
        GL_FALSE,
        glm::value_ptr(mat)
    );
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(
        glGetUniformLocation(ID, name.c_str()),
        1,
        glm::value_ptr(value)
    );
}