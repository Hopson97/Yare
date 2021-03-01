#include "InGameScreen.h"

#include "../ClientSettings.h"
#include "../Renderer/GLDebug.h"
#include "../Renderer/Mesh.h"
#include <SFML/Window/Keyboard.hpp>
#include <imgui/imgui.h>
#include <iostream>
#include <random>

struct Camera {
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0, 0, 0};
    glm::mat4 projectionMatrix{1.0f};

    void init(float aspectRatio, float fov);
    glm::mat4 getProjectionView() const;
} m_camera;

void Camera::init(float aspectRatio, float fov)
{
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 1000.0f);
}

glm::mat4 Camera::getProjectionView() const
{
    return projectionMatrix * createViewMartix(position, rotation);
}

struct Player {
    glm::vec3 position{100.0f, 0.0f, 100.0f};
    glm::vec3 rotation{0.0f, 180.0f, 0.0f};
    glm::vec3 velocity{0.0f};
} m_player;

InGameScreen::InGameScreen(ScreenManager& screens)
    : Screen(screens)
    , m_reflection(1600 / 2, 900 / 2)
    , m_refraction(1600 / 2, 900 / 2)
// , m_camera(1280.0f / 720.0f, 80)
{
    m_reflection.bind();
    m_reflection.attachTexture();
    m_reflection.finalise();

    m_refraction.bind();
    m_refraction.attachTexture();
    m_refraction.finalise();

    m_camera.init(1600.0f / 900.0f, 75);

    // Create a shader
    m_shader.addShader("Terrain", ShaderType::Vertex);
    m_shader.addShader("Terrain", ShaderType::Fragment);
    m_shader.linkShaders();
    m_shader.bind();
    m_shader.loadUniform("lightPosition", {200, 100, 200});

    // Create a shader
    m_waterShader.addShader("Water", ShaderType::Vertex);
    m_waterShader.addShader("Water", ShaderType::Fragment);
    m_waterShader.linkShaders();
    m_waterShader.bind();
    m_waterShader.loadUniform("lightPosition", {10, 100, 10});

    m_waterShader.loadUniform("reflectionSampler", 0);
    // m_waterShader.loadUniform("normalTexture", 1);
    // m_waterShader.loadUniform("displaceTexture", 2);

    auto cube = createCubeMesh({1, 1, 1});
    m_cubeVao.bind();
    m_cubeVao.addAttribute(cube.positions);
    m_cubeVao.addAttribute(cube.colours);
    m_cubeVao.addAttribute(cube.normals);
    m_cubeVao.addElements(cube.indices);

    auto terrain = createTerrainMesh(false);
    m_terrainVao.bind();
    m_terrainVao.addAttribute(terrain.positions);
    m_terrainVao.addAttribute(terrain.colours);
    m_terrainVao.addAttribute(terrain.normals);
    m_terrainVao.addElements(terrain.indices);

    auto water = createTerrainMesh(true);
    m_waterVao.bind();
    m_waterVao.addAttribute(water.positions);
    m_waterVao.addAttribute(water.colours);
    m_waterVao.addAttribute(water.normals);
  //  m_waterVao.addAttribute(water.textureCoords);
    m_waterVao.addElements(water.indices);

    std::mt19937 rng(std::time(nullptr));
    std::uniform_real_distribution<float> dist(0, 100);
    for (int i = 0; i < 100; i++) {
        m_cubePositions.emplace_back(
            std::make_pair(glm::vec3{dist(rng), dist(rng), dist(rng)},
                           glm::vec3{dist(rng), dist(rng), dist(rng)}));
    }

    // m_grassTexture.create("grass.png", true);
    m_waterTexture.create("water.jpeg", true);
    //    m_waterNormalTexture.create("waternormal.png", true);
    //  m_waterDisplaceTexture.create("waterdudv.png", true);
}

InGameScreen::~InGameScreen()
{
    glUseProgram(0);
    glBindVertexArray(0);
}

void InGameScreen::onInput(const sf::Window& window, const Keyboard& keyboard)
{
    if (!m_isPaused && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        m_isPaused = true;
    }
    if (m_isPaused) {
        return;
    }
    auto SPEED = 20.2f;
    static sf::Vector2i m_lastMousePosition;
    sf::Vector2i change = sf::Mouse::getPosition(window) - m_lastMousePosition;
    m_player.rotation.x += static_cast<float>(change.y / 8.0f * 0.5);
    m_player.rotation.y += static_cast<float>(change.x / 8.0f * 0.5);
    sf::Mouse::setPosition({(int)window.getSize().x / 2, (int)window.getSize().y / 2},
                           window);
    m_lastMousePosition = sf::Mouse::getPosition(window);

    // Inout for keyboard
    if (keyboard.isKeyDown(sf::Keyboard::W)) {
        m_player.velocity += forwardsVector(m_player.rotation) * SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::S)) {
        m_player.velocity += backwardsVector(m_player.rotation) * SPEED;
    }
    if (keyboard.isKeyDown(sf::Keyboard::A)) {
        m_player.velocity += leftVector(m_player.rotation) * SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::D)) {
        m_player.velocity += rightVector(m_player.rotation) * SPEED;
    }

    // m_firstPersonController.control(window, keyboard, m_player.velocity,
    //                                 m_player.rotation);
}

void InGameScreen::onUpdate(float dt)
{
    if (m_isPaused) {
        return;
    }
    m_player.position += m_player.velocity * dt;
    m_player.velocity *= 0.9;

    // Update camera
    m_camera.position = m_player.position;
    m_camera.rotation = m_player.rotation;
}

void InGameScreen::onRender(Framebuffer& framebuffer)
{
    m_shader.bind();

    // Render refraction texture
    m_refraction.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderScene({0, -1, 0, 0});

    // Render reflection texture
    m_refraction.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderScene({0, 1, 0, 0});

    // Render scene as normal
    framebuffer.bind();
    renderScene({0, -1, 0, 1000000});

    // Render the water
    m_waterShader.bind();
    auto modelmatrix = createModelMatrix({0, 0, 0}, {0, 0, 0});
    auto projectionView = m_camera.getProjectionView();
    m_waterShader.loadUniform("time", m_timer.getElapsedTime().asSeconds());
    m_waterShader.loadUniform("projectionViewMatrix", projectionView);
    m_waterShader.loadUniform("modelMatrix", modelmatrix);

    glActiveTexture(GL_TEXTURE0);
    m_reflection.bind();
   // m_waterTexture.bind();

    glCullFace(m_camera.position.y < 0 ? GL_FRONT : GL_BACK);
    m_waterVao.getDrawable().bindDrawElements();
    /*

        // Render water
        {
            // Render the reflection
            //    m_camera.position.y = -m_camera.position.y;
            // m_camera.rotation.x = -m_camera.rotation.x;
            auto reflectProjectionView = m_camera.getProjectionView();
            m_reflection.bind();
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            m_shader.loadUniform("projectionViewMatrix", reflectProjectionView);

            m_cubeVao.getDrawable().bind();

            for (auto& cube : m_cubePositions) {
                auto modelmatrix = createModelMatrix(cube.first, cube.second);
                m_shader.loadUniform("modelMatrix", modelmatrix);

                m_cubeVao.getDrawable().draw();
            }

            m_terrainVao.getDrawable().bindDrawElements();

            // Render the water using the reflection texture
            glActiveTexture(GL_TEXTURE0);
            m_reflection.bindTexture(0);
            framebuffer.bind();

            // m_camera.position.y = -m_camera.position.y;
            // m_camera.rotation.x = -m_camera.rotation.x;
            m_waterShader.bind();
            m_waterShader.loadUniform("time", m_timer.getElapsedTime().asSeconds());
            m_waterShader.loadUniform("modelMatrix", modelmatrix);
            m_waterShader.loadUniform("projectionViewMatrix", projectionView);

            glCullFace(m_camera.position.y < 0 ? GL_FRONT : GL_BACK);

            m_waterVao.getDrawable().bindDrawElements();
        }
    */
    glCullFace(GL_BACK);
    if (m_isPaused) {
        if (m_isSettingsOpened) {
            ClientSettings::get().showSettingsMenu([&] { m_isSettingsOpened = false; });
        }
        else {
            showPauseMenu();
        }
    }
}

void InGameScreen::renderScene(const glm::vec4& clippingPlane)
{
    auto projectionView = m_camera.getProjectionView();
    m_shader.loadUniform("projectionViewMatrix", projectionView);
    m_shader.loadUniform("clippingPlane", clippingPlane);

    // Render cubes
    {
        const Drawable& cubeDrawable = m_cubeVao.getDrawable();
        cubeDrawable.bind();
        for (auto& cube : m_cubePositions) {
            auto modelmatrix = createModelMatrix(cube.first, cube.second);
            cube.second.x++;
            cube.second.y++;
            cube.second.z++;
            m_shader.loadUniform("modelMatrix", modelmatrix);

            cubeDrawable.draw();
        }
    }
    // Render terrain
    {
        auto modelmatrix = createModelMatrix({0, 0, 0}, {0, 0, 0});
        m_shader.loadUniform("modelMatrix", modelmatrix);
        m_terrainVao.getDrawable().bindDrawElements();
    }
}

void InGameScreen::showPauseMenu()
{
    if (imguiBeginCustom("P A U S E D")) {
        if (imguiButtonCustom("Resume Game")) {
            m_isPaused = false;
        }
        if (imguiButtonCustom("Settings")) {
            m_isSettingsOpened = true;
        }
        if (imguiButtonCustom("Exit Game")) {
            m_pScreens->popScreen();
        }
    }
    ImGui::End();
}