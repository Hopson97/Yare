#include "Mesh.h"

#include "../../Maths.h"
#include <array>
#include <ctime>
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

        // Front, left, back, right, top, bottom

        // clang-format off
        mesh.positions.insert(mesh.positions.end(), {
            {w, h,    d}, {ox,  h,  d}, {ox, oy,  d}, {w,  oy,  d},
            {ox, h,   d}, {ox,  h, oz}, {ox, oy, oz}, {ox, oy,  d},
            {ox, h,  oz}, {w,   h, oz}, {w,  oy, oz}, {ox, oy, oz},
            {w, h,   oz}, {w,   h,  d}, {w,  oy,  d}, {w,  oy, oz},
            {w, h,   oz}, {ox,  h, oz}, {ox,  h,  d}, {w,   h,  d},
            {ox, oy, oz}, {w,  oy, oz}, {w,  oy,  d}, {ox, oy,  d}
        });

        //mesh.textureCoords.insert(mesh.textureCoords.end(), {
        //   {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},
        //   {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},
        //   {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},
        //   {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},
        //   {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},
        //   {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},
        //});

        mesh.colours.insert(mesh.colours.end(), {
           {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0},
           {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0},
           {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0},
           {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0},
           {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0},
           {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0},
        });
    
        mesh.normals.insert(mesh.normals.end(), {
            { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f},
            {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, 
            { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f},
            { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, 
            { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f},
            { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f},
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

float getNoiseAt(const glm::vec2& position, int seed)
{
    const float ROUGH = 0.7;
    const float SMOOTH = 250.0f;
    const int OCTAVES = 5;

    float vertexX = position.x;
    float vertexZ = position.y;

    float value = 0;
    float acc = 0;
    for (int i = 0; i < OCTAVES; i++) {
        float frequency = glm::pow(2.0f, i);
        float amplitude = glm::pow(ROUGH, i);

        float x = vertexX * frequency / SMOOTH;
        float z = vertexZ * frequency / SMOOTH;

        float noiseValue = glm::simplex(glm::vec3{x, z, seed});
        noiseValue = (noiseValue + 1.0f) / 2.0f;
        value += noiseValue * amplitude;
        acc += amplitude;
    }
    return value / acc * 50 - 30;
}

float getNoiseAt2(const glm::vec2& position, int seed)
{
    const float ROUGH = 1.2;
    const float SMOOTH = 50.0f;
    const int OCTAVES = 5;

    float vertexX = position.x;
    float vertexZ = position.y;

    float value = 0;
    float acc = 0;
    for (int i = 0; i < OCTAVES; i++) {
        float frequency = glm::pow(2.0f, i);
        float amplitude = glm::pow(ROUGH, i);

        float x = vertexX * frequency / SMOOTH;
        float z = vertexZ * frequency / SMOOTH;

        float noiseValue = glm::simplex(glm::vec3{x, z, seed});
        noiseValue = (noiseValue + 1.0f) / 2.0f;
        value += noiseValue * amplitude;
        acc += amplitude;
    }
    return value / acc * 5 - 4;
}

Mesh createTerrainMesh(bool isWater)
{
    int seed = std::time(nullptr) / 100000;
    if (!isWater) {
        // 16145
        std::cout << "Seed: " << seed << std::endl;
    }
    constexpr float SIZE = 256;
    constexpr float VERTS = 256;
    constexpr unsigned TOTAL_VERTS = VERTS * VERTS;

    std::vector<float> heights(TOTAL_VERTS);

    if (!isWater) {
        for (int y = 0; y < VERTS; y++) {
            for (int x = 0; x < VERTS; x++) {
                heights[y * VERTS + x] =
                    getNoiseAt({x, y}, seed) + getNoiseAt2({x, y}, seed);
            }
        }
    }
    else {
        std::fill(heights.begin(), heights.end(), 0);
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

            float vx = fx / (VERTS - 1) * SIZE;
            float vz = fy / (VERTS - 1) * SIZE;
            float vy = getHeight(x, y);
            terrain.positions.emplace_back(vx, vy, vz);

            float h1 = getHeight(x - 1, y);
            float h2 = getHeight(x + 1, y);
            float h3 = getHeight(x, y - 1);
            float h4 = getHeight(x, y + 1);
            glm::vec3 normal{h1 - h2, 2, h3 - h4};
            glm::vec3 n = glm::normalize(normal);
            terrain.normals.emplace_back(n.x, n.y, n.z);

            Mesh::Colour colour;

            if (isWater) {
                colour.r = 69;
                colour.g = 255;
                colour.b = 241;
                colour.a = 200;
            }
            else {
                int height = static_cast<int>(vy);
                if (height > 10) {
                    colour = Mesh::Colour{255, 255, 255};
                }
                else if (height > 6) {
                    colour = Mesh::Colour{100, 100, 100};
                }
                else if (height > 0) {
                    colour.g = 255;
                }
                else if (height > -3) {
                    colour.r = 255;
                    colour.g = 220;
                    colour.b = 127;
                }
                else {
                    colour = Mesh::Colour{100, 100, 100};
                }
            }
            terrain.colours.emplace_back(colour);

            // float u = fx / VERTS - 1; // y % (int)VERTS;
            // float v = fy / VERTS - 1; // x % (int)VERTS;

            // float u = y % (int)VERTS;
            // float v = x % (int)VERTS;
            // terrain.textureCoords.emplace_back(u, v);
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
