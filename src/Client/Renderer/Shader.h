#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

enum class ShaderType {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    TessellationControl = GL_TESS_CONTROL_SHADER,
    TessellationEval = GL_TESS_EVALUATION_SHADER,

};

class Shader final {
  public:
    Shader() = default;
    Shader& operator=(Shader&& other) noexcept;
    Shader(Shader&& other) noexcept;
    ~Shader();

    Shader& operator=(Shader& other) = delete;
    Shader(Shader& other) = delete;

    void bind();
    void addShader(const std::string& filename, ShaderType type);
    void linkShaders();

    void loadUniform(const char* name, int value);
    void loadUniform(const char* name, float value);
    void loadUniform(const char* name, const glm::mat4& matrix);
    void loadUniform(const char* name, const glm::vec4& vector);
    void loadUniform(const char* name, const glm::vec3& vector);

  private:
    GLuint getUniformLocation(const char* name);

    std::unordered_map<const char*, GLuint> m_uniformLocations;
    std::vector<GLuint> m_stages;
    GLuint m_program = 0;
};
