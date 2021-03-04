#pragma once

#include <glad/glad.h>

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Colour.h"
#include "VertexArray.h"

constexpr int TERRAIN_SIZE = 128;

struct Mesh final {
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

struct Terrain {
    struct LOD {
        GLuint start = 0;
        GLuint count = 0;
    };
    std::vector<LOD> lods;
    glm::vec2 position;
    VertexArray vao;

    void createTerrainMesh(bool createBumps);
    void render(int LOD);
};

Mesh createCubeMesh(const glm::vec3& dimensions);
Mesh createWireCubeMesh(const glm::vec3& dimensions, float wireThickness = 0.1);
