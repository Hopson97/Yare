#include "Mesh.h"

#include "../../Maths.h"
#include <array>
#include <ctime>
#include <glm/gtc/noise.hpp>
#include <iomanip>
#include <iostream>
#include <random>

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

struct NoiseOptions {
    float roughness;
    float smoothness;
    float amplitude;

    int octaves;
    float offset;
};

float getNoiseAt(const glm::ivec2& position, int seed, NoiseOptions& options)
{
    float value = 0;
    float acc = 0;
    for (int i = 0; i < options.octaves; i++) {
        float frequency = glm::pow(2.0f, i);
        float amplitude = glm::pow(options.roughness, i);

        float x = position.x * frequency / options.smoothness;
        float z = position.y * frequency / options.smoothness;

        float noiseValue = glm::simplex(glm::vec3{x, z, seed});
        noiseValue = (noiseValue + 1.0f) / 2.0f;
        value += noiseValue * amplitude;
        acc += amplitude;
    }
    return value / acc * options.amplitude + options.offset;
}

void Terrain::createTerrainMesh(bool isWater)
{
    NoiseOptions terrainNoise;
    terrainNoise.roughness = 0.7;
    terrainNoise.smoothness = 350.0f;
    terrainNoise.octaves = 5;
    terrainNoise.amplitude = 80.0f;
    terrainNoise.offset = -47;

    NoiseOptions bumpNoise;
    bumpNoise.roughness = 1.2;
    bumpNoise.smoothness = 50.0f;
    bumpNoise.octaves = 5;
    bumpNoise.amplitude = 5.0f;
    bumpNoise.offset = -4;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(-20000, 20000);
    int seed = 13913; // dist(rng);
    if (!isWater) {
        // 16145
        // 18090
        // -7112
        // 13913
        std::cout << "Seed: " << seed << " " << RAND_MAX << std::endl;
    }
    constexpr float SIZE = TERRAIN_SIZE;
    constexpr float VERTS = TERRAIN_SIZE;
    constexpr int HEIGHT_MAPS_WIDTH = VERTS + 2;
    constexpr unsigned HEIGHT_MAP_SIZE = HEIGHT_MAPS_WIDTH * HEIGHT_MAPS_WIDTH;

    std::vector<float> heights(HEIGHT_MAP_SIZE);

    if (!isWater) {
        for (int y = 0; y < HEIGHT_MAPS_WIDTH; y++) {
            for (int x = 0; x < HEIGHT_MAPS_WIDTH; x++) {
                int tx = x + (HEIGHT_MAPS_WIDTH - 1) * position.x + position.x * -2;
                int ty = y + (HEIGHT_MAPS_WIDTH - 1) * position.y + position.y * -2;
                float height = getNoiseAt({tx, ty}, seed, terrainNoise);
                float bumps = getNoiseAt({tx, ty}, seed, bumpNoise);
                heights[y * HEIGHT_MAPS_WIDTH + x] = height + bumps;
            }
        }
    }
    else {
        std::fill(heights.begin(), heights.end(), 0);
    }
    auto getHeight = [&](int x, int y) {
        if (x < 0 || x >= HEIGHT_MAPS_WIDTH || y < 0 || y >= HEIGHT_MAPS_WIDTH) {
            return 0.0f;
        }
        else {
            return heights[y * HEIGHT_MAPS_WIDTH + x];
        }
    };

    Mesh mesh;
    for (int y = 0; y < VERTS; y++) {
        for (int x = 0; x < VERTS; x++) {
            auto fx = static_cast<float>(x);
            auto fy = static_cast<float>(y);

            float vx = fx / (VERTS - 1) * SIZE;
            float vz = fy / (VERTS - 1) * SIZE;

            int hx = x + 1;
            int hy = y + 1;
            float vy = getHeight(hx, hy);

            mesh.positions.emplace_back(vx, vy, vz);

            float h1 = getHeight(hx - 1, hy);
            float h2 = getHeight(hx + 1, hy);
            float h3 = getHeight(hx, hy - 1);
            float h4 = getHeight(hx, hy + 1);
            glm::vec3 normal{h1 - h2, 2, h3 - h4};
            glm::vec3 n = glm::normalize(normal);
            mesh.normals.emplace_back(n.x, n.y, n.z);

            Colour colour;

            if (isWater) {
                colour.r = 69;
                colour.g = 255;
                colour.b = 241;
                colour.a = 250;
            }
            else {
                int height = static_cast<int>(vy);
                if (height > 10) {
                    colour = {255, 255, 255};
                }
                else if (height > 6) {
                    colour = {100, 100, 100};
                }
                else if (height > 0) {
                    colour.g = 255;
                }
                else if (height > -4) {
                    colour.r = 255;
                    colour.g = 220;
                    colour.b = 127;
                }
                else {
                    colour.r = 255 / 4;
                    colour.g = 220 / 4;
                    colour.b = 127 / 4;
                }
            }
            mesh.colours.emplace_back(colour);

            if (isWater) {
                float u = fx / VERTS - 1;
                float v = fy / VERTS - 1;

                // float u = y % (int)VERTS;
                // float v = x % (int)VERTS;
                mesh.textureCoords.emplace_back(u, v);
            }

            // float u = fx / VERTS - 1; // y % (int)VERTS;
            // float v = fy / VERTS - 1; // x % (int)VERTS;

            // float u = y % (int)VERTS;
            // float v = x % (int)VERTS;
            // terrain.textureCoords.emplace_back(u, v);
        }
    }

    auto createLOD = [&](int level) {
        LOD& lod = lods.emplace_back();
        lod.start = mesh.indices.size();
        for (int y = 0; y < (VERTS - level); y += level) {
            for (int x = 0; x < (VERTS - level); x += level) {
                int topLeft = (y * VERTS) + x;
                int topRight = topLeft + level;
                int bottomLeft = ((y + level) * VERTS) + x;
                int bottomRight = bottomLeft + level;

                mesh.indices.push_back(topLeft);
                mesh.indices.push_back(bottomLeft);
                mesh.indices.push_back(topRight);
                mesh.indices.push_back(bottomLeft);
                mesh.indices.push_back(bottomRight);
                mesh.indices.push_back(topRight);

                lod.count += 6;
            }
        }
    };
    createLOD(1);
    createLOD(2);
    createLOD(4);
    createLOD(8);
    createLOD(16);
    createLOD(32);
    createLOD(64);
    createLOD(TERRAIN_SIZE);

    vao.bind();
    vao.addAttribute(mesh.positions);
    vao.addAttribute(mesh.colours);
    vao.addAttribute(mesh.normals);
    if (false) {
        vao.addAttribute(mesh.textureCoords);
    }
    vao.addElements(mesh.indices);
}

void Terrain::render(int LOD)
{
    LOD += 1;
    GLuint count = lods[LOD - 1].count;
    GLuint start = lods[LOD - 1].start;
    vao.getDrawable().bindDrawElements(count, start);
}
