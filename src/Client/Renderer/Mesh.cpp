#include "Mesh.h"

#include "../../Maths.h"
#include <array>
#include <glm/gtc/noise.hpp>
#include <iostream>

namespace {
    void addCubeToMesh(Mesh& mesh, const glm::vec3& dimensions,
                       const glm::vec3& offset = {0, 0, 0})
    {
        float w = dimensions.x + offset.x;
        float h = dimensions.y + offset.y;
        float d = dimensions.z + offset.z;

        float ox = offset.x;
        float oy = offset.y;
        float oz = offset.z;

        // clang-format off
        mesh.positions.insert(mesh.positions.end(), {
            // Front of the cube
            w, h, d,    ox, h, d,   ox, oy, d,  w, oy, d,
            // Left
            ox, h, d,   ox, h, oz,  ox, oy, oz, ox, oy, d,
            // Back
            ox, h, oz,  w, h, oz,   w, oy, oz,  ox, oy, oz,
            // Right
            w, h, oz,   w, h, d,    w, oy, d,   w, oy, oz,
            // Top
            w, h, oz,   ox, h, oz,  ox, h, d,   w, h, d,
            // Bottom
            ox, oy, oz, w, oy, oz,  w, oy, d,   ox, oy, d
        });

        mesh.textureCoords.insert(mesh.textureCoords.end(), {
            // Front of the cube
           0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
            // Left
            0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
            // Back
            0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
            // Right
            0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
            // Top
            0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
            // Bottom
            0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        });
    
        mesh.normals.insert(mesh.normals.end(), {
            // Front of the cube
            0, 0, 1.f,    0, 0, 1.f,    0, 0, 1.f,     0, 0, 1.0f,
            // Left
            -1.f, 0, 0,  -1.f, 0, 0,   -1.f, 0, 0,    -1.f, 0, 0, 
            // Back
            0, 0, -1.f,   0, 0, -1.f,   0, 0, -1.f,    0, 0, -1.0f,
            // Right
            1.f, 0, 0,    1.f, 0, 0,    1.f, 0, 0,     1.f, 0, 0, 
            // Top
            0, 1.f, 0,    0, 1.f, 0,    0, 1.f, 0,     0, 1.f, 0,
            // Bottom
            0, -1.f, 0,   0, -1.f, 0,   0, -1.f, 0,    0, -1.f, 0,
        });
        // clang-format on

        // For each cube face, add indice
        for (int i = 0; i < 6; i++) {
            mesh.indices.push_back(mesh.currentIndex);
            mesh.indices.push_back(mesh.currentIndex + 1);
            mesh.indices.push_back(mesh.currentIndex + 2);
            mesh.indices.push_back(mesh.currentIndex + 2);
            mesh.indices.push_back(mesh.currentIndex + 3);
            mesh.indices.push_back(mesh.currentIndex);
            mesh.currentIndex += 4;
        }
    }
} // namespace

Mesh createCubeMesh(const glm::vec3& dimensions)
{
    Mesh cube;
    addCubeToMesh(cube, dimensions);
    return cube;
}

Mesh createWireCubeMesh(const glm::vec3& dimensions, float wireThickness)
{
    Mesh cube;
    float w = dimensions.x;
    float h = dimensions.y;
    float d = dimensions.z;
    // Front
    addCubeToMesh(cube, {w, wireThickness, wireThickness});
    addCubeToMesh(cube, {w, wireThickness, wireThickness}, {0, h, 0});
    addCubeToMesh(cube, {wireThickness, h, wireThickness});
    addCubeToMesh(cube, {wireThickness, h, wireThickness}, {w, 0, 0});

    // Back
    addCubeToMesh(cube, {w, wireThickness, wireThickness}, {0, 0, d});
    addCubeToMesh(cube, {w, wireThickness, wireThickness}, {0, h, d});
    addCubeToMesh(cube, {wireThickness, h, wireThickness}, {0, 0, d});
    addCubeToMesh(cube, {wireThickness, h, wireThickness}, {w, 0, d});

    // Right
    addCubeToMesh(cube, {wireThickness, wireThickness, d}, {0, h, 0});
    addCubeToMesh(cube, {wireThickness, wireThickness, d});

    // Left
    addCubeToMesh(cube, {wireThickness, wireThickness, d}, {w, h, 0});
    addCubeToMesh(cube, {wireThickness, wireThickness, d}, {w, 0, 0});

    return cube;
}

float getNoiseAt(float z, float vx, float vz)
{
    const float ROUGH = 0.6f;
    const float SMOOTH = 300.0f;
    const int OCTAVES = 7;

    float vertexX = vx;
    float vertexZ = vz + z;

    float value = 0;
    float acc = 0;
    for (int i = 0; i < OCTAVES; i++) {
        float freq = glm::pow(2.0f, i);
        float amps = glm::pow(ROUGH, i);

        float x = vertexX * freq / SMOOTH;
        float z = vertexZ * freq / SMOOTH;

        float noiseValue = glm::simplex(glm::vec2{x, z});
        noiseValue = (noiseValue + 1.0f) / 2.0f;
        value += noiseValue * amps;
        acc += amps;
    }
    return value / acc;
}

Mesh createTerrainMesh()
{
    constexpr float SIZE = 256;
    constexpr float VERTS = 256;
    constexpr unsigned TOTAL_VERTS = VERTS * VERTS;

    std::array<float, TOTAL_VERTS> heights;
    for (int y = 0; y < VERTS; y++) {
        for (int x = 0; x < VERTS; x++) {
            heights[y * VERTS + x] = getNoiseAt(0, (float)x, (float)y) * 100.0f - 110;
        }
    }

    auto getHeight = [&](int x, int y) {
        if (x < 0 || x >= VERTS || y < 0 || y >= VERTS) {
            return 0.0f;
        }
        else {
            return heights[y * (int)VERTS + x];
        }
    };

    Mesh terrain;
    for (int y = 0; y < VERTS; y++) {
        for (int x = 0; x < VERTS; x++) {
            auto fx = static_cast<float>(x);
            auto fy = static_cast<float>(y);

            terrain.positions.push_back(fx / (VERTS - 1) * SIZE);
            terrain.positions.push_back(getHeight(x, y));
            terrain.positions.push_back(fy / (VERTS - 1) * SIZE);

            float h1 = getHeight(x - 1, y);
            float h2 = getHeight(x + 1, y);
            float h3 = getHeight(x, y - 1);
            float h4 = getHeight(x, y + 1);

            glm::vec3 normal{h1 - h2, 2, h3 - h4};
            auto n = glm::normalize(normal);

            terrain.normals.push_back(n.x);
            terrain.normals.push_back(n.y);
            terrain.normals.push_back(n.z);

            terrain.textureCoords.push_back(y % (int)VERTS);
            terrain.textureCoords.push_back(x % (int)VERTS);
        }
    }

    for (int y = 0; y < VERTS - 1; y++) {
        for (int x = 0; x < VERTS - 1; x++) {
            int topLeft = (y * VERTS) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((y + 1) * VERTS) + x;
            int bottomRight = bottomLeft + 1;

            terrain.indices.push_back(topLeft);
            terrain.indices.push_back(bottomLeft);
            terrain.indices.push_back(topRight);
            terrain.indices.push_back(topRight);
            terrain.indices.push_back(bottomLeft);
            terrain.indices.push_back(bottomRight);
        }
    }
    return terrain;
}
