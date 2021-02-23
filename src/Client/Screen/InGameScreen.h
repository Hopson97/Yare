#pragma once

#include "../../Maths.h"
#include "../Input.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../Renderer/VertexArray.h"
#include "Screen.h"
#include <SFML/System/Clock.hpp>

class InGameScreen final : public Screen {
  public:
    InGameScreen(ScreenManager& stack);
    ~InGameScreen();

    void onInput(const sf::Window& window, const Keyboard& keyboard) override;
    void onUpdate(float dt) override;
    void onRender() override;

  private:
    bool m_isPaused = false;
    bool m_isSettingsOpened = false;
    void showPauseMenu();

    FirstPersonController m_firstPersonController;

    Texture2d m_texture;
    VertexArray m_cubeVao;
    VertexArray m_terrainVao;
    Shader m_shader;

    std::vector<std::pair<glm::vec3, glm::vec3>> m_cubePositions;
};