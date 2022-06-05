#pragma once

#include <glad/glad.h>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "Colour.h"

enum DrawMode {
    Triangles = GL_TRIANGLES,
};

enum PolygonMode {
    Wireframe = GL_LINE,
    Fill = GL_FILL,
};

struct Drawable final {
    const GLuint vertexArray = 0;
    const GLsizei indicesCount = 0;

    Drawable(GLuint vao, GLsizei indices);

    void bind() const;

    void drawArrays(int count = 6, DrawMode mode = DrawMode::Triangles) const;

    void draw(int count, GLuint start = 0, DrawMode mode = DrawMode::Triangles) const;

    void bindDrawElements(int count, GLuint start = 0,
                          DrawMode mode = DrawMode::Triangles) const;

    void draw(GLuint start = 0, DrawMode mode = DrawMode::Triangles) const;

    void bindDrawElements(GLuint start = 0, DrawMode mode = DrawMode::Triangles) const;
};

class VertexArray final {
  public:
    VertexArray();
    VertexArray& operator=(VertexArray&& other) noexcept;
    VertexArray(VertexArray&& other) noexcept;
    ~VertexArray();

    Drawable getDrawable() const;

    void bind() const;
    GLsizei indicesCount() const;

    void addAttribute(const std::vector<glm::vec2>& data);
    void addAttribute(const std::vector<glm::vec3>& data);
    void addAttribute(const std::vector<Colour>& data);

    //  void addAttribute(const std::vector<GLfloat>& data, int nPerVertex);
    //  void addAttribute(const std::vector<GLuint>& data, int nPerVertex);
    void addElements(const std::vector<GLuint>& indices);

    VertexArray& operator=(VertexArray& other) = delete;
    VertexArray(VertexArray& other) = delete;

  private:
    std::vector<GLuint> m_vertexBuffers;
    GLuint m_vertexArray = 0;
    GLsizei m_indicesCount = 0;
};

void setPolygonMode(PolygonMode mode);
