#include "VertexArray.h"

#include "GLDebug.h"
#include <cassert>
#include <iostream>

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_vertexArray);
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    m_vertexArray = other.m_vertexArray;
    m_vertexBuffers = std::move(other.m_vertexBuffers);
    m_indicesCount = other.m_indicesCount;

    other.m_vertexArray = 0;
    other.m_indicesCount = 0;

    return *this;
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_vertexBuffers{std::move(other.m_vertexBuffers)}
    , m_vertexArray{other.m_vertexArray}
    , m_indicesCount{other.m_indicesCount}
{
    other.m_vertexArray = 0;
    other.m_indicesCount = 0;
}

VertexArray::~VertexArray()
{
    if (m_vertexArray) {
        glDeleteVertexArrays(1, &m_vertexArray);
        glDeleteBuffers(m_vertexBuffers.size(), m_vertexBuffers.data());
    }
}

Drawable VertexArray::getDrawable() const
{
    return {m_vertexArray, m_indicesCount};
}

void VertexArray::bind() const
{
    glBindVertexArray(m_vertexArray);
}

GLsizei VertexArray::indicesCount() const
{
    return m_indicesCount;
}

void VertexArray::addAttribute(const std::vector<glm::vec2>& data)
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0].x) * 2, data.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(m_vertexBuffers.size(), 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(m_vertexBuffers.size());
    m_vertexBuffers.push_back(vbo);
}

void VertexArray::addAttribute(const std::vector<glm::vec3>& data)
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0].x) * 3, data.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(m_vertexBuffers.size(), 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(m_vertexBuffers.size());
    m_vertexBuffers.push_back(vbo);
}

void VertexArray::addAttribute(const std::vector<Colour>& data)
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]) * 4, data.data(),
                 GL_STATIC_DRAW);

    // TODO See if nullptr works in place of the GLvoid cast
    glVertexAttribPointer(m_vertexBuffers.size(), 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,
                          (GLvoid*)0);
    glEnableVertexAttribArray(m_vertexBuffers.size());
    m_vertexBuffers.push_back(vbo);
}

// void VertexArray::addAttribute(const std::vector<GLuint>& data, int nPerVertex)
//{
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(),
//                 GL_STATIC_DRAW);
//    glVertexAttribIPointer(m_vertexBuffers.size(), nPerVertex, GL_UNSIGNED_INT, 0,
//                           (GLvoid*)0);
//    glEnableVertexAttribArray(m_vertexBuffers.size());
//    m_vertexBuffers.push_back(vbo);
//}

void VertexArray::addElements(const std::vector<GLuint>& indices)
{
    GLuint elementVbo;
    glGenBuffers(1, &elementVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]),
                 indices.data(), GL_STATIC_DRAW);
    m_vertexBuffers.push_back(elementVbo);
    m_indicesCount = indices.size();
}

Drawable::Drawable(GLuint vao, GLsizei indices)
    : vertexArray{vao}
    , indicesCount{indices}
{
}

void Drawable::draw(DrawMode mode) const
{
    glDrawElements(mode, indicesCount, GL_UNSIGNED_INT, nullptr);
}

void Drawable::drawArrays(int count, DrawMode mode) const
{
    glDrawArrays(mode, 0, count);
}

void Drawable::bind() const
{
    assert(vertexArray != 0);
    glBindVertexArray(vertexArray);
}

void setPolygonMode(PolygonMode mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, mode);
}