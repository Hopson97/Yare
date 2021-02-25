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
    glm::vec3 position{0.0f, 10.0f, 0.0f};
    glm::vec3 rotation{0.0f, 180.0f, 0.0f};
    glm::vec3 velocity{0.0f};
} m_player;

InGameScreen::InGameScreen(ScreenManager& screens)
    : Screen(screens)
// , m_camera(1280.0f / 720.0f, 80)
{

    m_camera.init(1600.0f / 900.0f, 75);

    // Create a shader
    m_shader.addShader("Static", ShaderType::Vertex);
    m_shader.addShader("Static", ShaderType::Fragment);
    m_shader.linkShaders();
    m_shader.bind();
    m_shader.loadUniform("lightPosition", {10, 100, 10});

    // Create a shader
    m_waterShader.addShader("Static", ShaderType::Vertex);
    m_waterShader.addShader("Normal", ShaderType::Fragment);
    m_waterShader.linkShaders();
    m_waterShader.bind();
    m_waterShader.loadUniform("lightPosition", {10, 100, 10});

    auto cube = createCubeMesh({1, 1, 1});
    m_cubeVao.bind();
    m_cubeVao.addAttribute(cube.positions);
    m_cubeVao.addAttribute(cube.textureCoords);
    m_cubeVao.addAttribute(cube.normals);
    m_cubeVao.addElements(cube.indices);

    auto terrain = createTerrainMesh(true);
    m_terrainVao.bind();
    m_terrainVao.addAttribute(terrain.positions);
    m_terrainVao.addAttribute(terrain.textureCoords);
    m_terrainVao.addAttribute(terrain.normals);
    m_terrainVao.addElements(terrain.indices);

    auto water = createTerrainMesh(false);
    m_waterVao.bind();
    m_waterVao.addAttribute(water.positions);
    m_waterVao.addAttribute(water.textureCoords);
    m_waterVao.addAttribute(water.normals);
    m_waterVao.addElements(water.indices);

    std::mt19937 rng(std::time(nullptr));
    std::uniform_real_distribution<float> dist(0, 100);
    for (int i = 0; i < 100; i++) {
        m_cubePositions.emplace_back(
            std::make_pair(glm::vec3{dist(rng), dist(rng), dist(rng)},
                           glm::vec3{dist(rng), dist(rng), dist(rng)}));
    }

    m_grassTexture.create("grass.png", true);
    m_waterTexture.create("water.jpg", true);
    m_waterTextureNormal.create("water_norm.jpg", true);
    m_waterShader.loadUniform("tex", 0);
    m_waterShader.loadUniform("norm", 1);
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
    auto SPEED = 0.50f;
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

void InGameScreen::onRender()
{
    glActiveTexture(GL_TEXTURE0);
    m_grassTexture.bind();
    m_shader.bind();

    // Load up projection matrix stuff
    auto projectionView = m_camera.getProjectionView();
    m_shader.loadUniform("projectionViewMatrix", projectionView);

    m_cubeVao.getDrawable().bind();

    for (auto& cube : m_cubePositions) {
        auto modelmatrix = createModelMatrix(cube.first, cube.second);
        cube.second.x++;
        cube.second.y++;
        cube.second.z++;
        m_shader.loadUniform("modelMatrix", modelmatrix);

        m_cubeVao.getDrawable().draw();
    }
    // Render terrain and water
    auto modelmatrix = createModelMatrix({-50, 0, -50}, {0, 0, 0});
    m_shader.loadUniform("modelMatrix", modelmatrix);
    m_terrainVao.getDrawable().bind();
    m_terrainVao.getDrawable().draw();

    m_waterShader.bind();
    m_waterShader.loadUniform("modelMatrix", modelmatrix);
    m_waterShader.loadUniform("projectionViewMatrix", projectionView);

    glActiveTexture(GL_TEXTURE0);
    m_waterTexture.bind();
    glActiveTexture(GL_TEXTURE1);
    m_waterTextureNormal.bind();

    m_waterVao.getDrawable().bind();
    m_waterVao.getDrawable().draw();



    if (m_isPaused) {
        if (m_isSettingsOpened) {
            ClientSettings::get().showSettingsMenu([&] { m_isSettingsOpened = false; });
        }
        else {
            showPauseMenu();
        }
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