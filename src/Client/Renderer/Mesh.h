#pragma once

#include <glad/glad.h>

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Mesh final {
    struct Colour {
        Colour() = default;

        Colour(uint8_t r, uint8_t g, uint8_t b)
            : r{r}
            , g{g}
            , b{b}
        {
        }

        Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
            : r{r}
            , g{g}
            , b{b}
            , a{a}
        {
        }

        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;
    };
    /*
    struct Vertex {
        glm::vec3 position;
        glm::vec3 texCoord;
        glm::vec3 normal;
    };**/

    std::vector<glm::vec3> positions;
    std::vector<Colour> colours;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    int currentIndex = 0;
};

Mesh createCubeMesh(const glm::vec3& dimensions);
Mesh createWireCubeMesh(const glm::vec3& dimensions, float wireThickness = 0.1);
Mesh createTerrainMesh(bool createBumps);
