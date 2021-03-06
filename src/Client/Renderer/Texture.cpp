#include "Texture.h"

#include "GLDebug.h"
#include <SFML/Graphics/Image.hpp>
#include <iostream>
#include <stdexcept>

namespace {
    GLuint createTexture()
    {
        GLuint handle;
        glGenTextures(1, &handle);
        glActiveTexture(GL_TEXTURE0);
        return handle;
    }

    void destroyTexture(GLuint* texture)
    {
        if (*texture) {
            glDeleteTextures(1, texture);
            *texture = 0;
        }
    }

} // namespace

///========================================
///
///             Texture2d
///
///========================================
Texture2d ::Texture2d()
    : m_handle{createTexture()}
{
}

Texture2d& Texture2d ::operator=(Texture2d&& other) noexcept
{
    m_handle = other.m_handle;
    other.m_handle = 0;
    return *this;
}

Texture2d ::Texture2d(Texture2d&& other) noexcept
    : m_handle{other.m_handle}
{
    other.m_handle = 0;
}

Texture2d ::~Texture2d()
{
    destroyTexture(&m_handle);
}

void Texture2d::create(unsigned width, unsigned height)
{
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture2d::create(const std::string& name, bool flip)
{
    sf::Image image;
    if (!image.loadFromFile("Data/Textures/" + name)) {
        throw std::runtime_error("Could not load texture.");
    }
    if (flip) {
        image.flipVertically();
    }
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
}

void Texture2d::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

GLuint Texture2d::textureId() const
{
    return m_handle;
}

///========================================
///
///             Texture Array
///
///========================================
TextureArray::TextureArray()
    : m_handle{createTexture()}
{
}

TextureArray::~TextureArray()
{
    destroy();
}

TextureArray::TextureArray(TextureArray&& other)
    : m_textureNames{std::move(other.m_textureNames)}
    , m_handle{other.m_handle}
    , m_maxTextures{other.m_maxTextures}
    , m_textureSize{other.m_textureSize}
{
    other.reset();
}

TextureArray& TextureArray::operator=(TextureArray&& other)
{
    destroy();
    m_textureNames = std::move(other.m_textureNames);
    m_handle = other.m_handle;
    m_maxTextures = other.m_maxTextures;
    m_textureSize = other.m_textureSize;
    other.reset();
    return *this;
}

void TextureArray::create(GLsizei numTextures, GLsizei textureSize)
{
    if (!m_handle) {
        m_handle = createTexture();
    }
    bind();

    m_maxTextures = numTextures;
    m_textureSize = textureSize;

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, textureSize, textureSize, numTextures,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

GLuint TextureArray::addTexture(const std::string& name)
{
    sf::Image image;
    if (!image.loadFromFile("Data/Textures/" + name)) {
        // Create a error image
        image.create(m_textureSize, m_textureSize);
        for (GLuint y = 0; y < m_textureSize; y++) {
            for (GLuint x = 0; x < m_textureSize; x++) {
                uint8_t r = static_cast<uint8_t>(rand() % 255);
                uint8_t g = static_cast<uint8_t>(rand() % 255);
                uint8_t b = static_cast<uint8_t>(rand() % 255);
                image.setPixel(x, y, {r, g, b});
            }
        }
    };

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_textureNames.size(), m_textureSize,
                    m_textureSize, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

    // Generate Mipmap
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, -1);

    GLuint id = m_textureNames.size();
    m_textureNames.emplace(name, id);

    return m_textureNames.size() - 1;
}

GLuint TextureArray::getTextureId(const std::string& name)
{
    auto itr = m_textureNames.find(name);
    if (itr == m_textureNames.end()) {
        return addTexture(name);
    }
    return itr->second;
}

void TextureArray::destroy()
{
    destroyTexture(&m_handle);
    m_textureNames.clear();
    m_maxTextures = 0;
    m_textureSize = 0;
}

void TextureArray::bind() const
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle);
}

void TextureArray::reset()
{
    m_handle = 0;
    m_textureNames.clear();
    m_maxTextures = 0;
    m_textureSize = 0;
}
