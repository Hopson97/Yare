#include "Shader.h"

#include "GLDebug.h"
#include <cassert>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace {
    std::string loadFile(const std::string fName)
    {
        std::ifstream inFile(fName);
        if (!inFile.is_open()) {
            std::cerr << "Unable to open file " << fName << std::endl;
        }
        std::ostringstream stream;
        stream << inFile.rdbuf();
        return stream.str();
    }
} // namespace

Shader& Shader::operator=(Shader&& other) noexcept
{
    m_program = other.m_program;
    other.m_program = 0;
    return *this;
}

Shader::Shader(Shader&& other) noexcept
    : m_program{other.m_program}
{
    other.m_program = 0;
}

Shader::~Shader()
{
    if (m_program != 0) {
        glDeleteProgram(m_program);
    }
}

void Shader::bind()
{
    assert(m_program != 0);
    glUseProgram(m_program);
}

void Shader::addShader(const std::string& filename, ShaderType type)
{
    std::string ext = (type == ShaderType::Vertex ? ".vert" : ".frag");
    std::string source = loadFile("Data/Shaders/" + filename + ext);
    auto shaderId = glCreateShader(static_cast<GLenum>(type));

    const GLchar* const shaderSourcePtr = source.data();
    const GLint shaderSourceLength = source.length();
    glShaderSource(shaderId, 1, &shaderSourcePtr, &shaderSourceLength);
    glCompileShader(shaderId);

    GLint logLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength) {
        std::string infoLog(logLength, 0);
        glGetShaderInfoLog(shaderId, logLength, nullptr, infoLog.data());
        std::cerr << "Error for " << ext << " shader: " << infoLog << std::endl;
        throw std::runtime_error(infoLog);
    }

    m_stages.push_back(shaderId);
}

void Shader::linkShaders()
{
    m_program = glCreateProgram();

    for (auto& stage : m_stages) {
        glAttachShader(m_program, stage);
    }

    glLinkProgram(m_program);

    for (auto& stage : m_stages) {
        glDetachShader(m_program, stage);
    }

    GLint logLength;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength) {
        std::string infoLog(logLength, 0);
        glGetProgramInfoLog(m_program, logLength, nullptr, infoLog.data());
        throw std::runtime_error(infoLog);
    }
}

void loadUniform(const char* name, int value);
void loadUniform(const char* name, const glm::mat4& matrix);
void loadUniform(const char* name, const glm::vec3& vector);

GLuint Shader::getUniformLocation(const char* name)
{
    auto itr = m_uniformLocations.find(name);
    if (itr == m_uniformLocations.cend()) {
        GLint location = glGetUniformLocation(m_program, name);
        assert(location != -1);
        m_uniformLocations.emplace(name, location);
    }
    return m_uniformLocations[name];
}

void Shader::loadUniform(const char* name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::loadUniform(const char* name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::loadUniform(const char* name, const glm::vec3& vector)
{
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vector));
}
