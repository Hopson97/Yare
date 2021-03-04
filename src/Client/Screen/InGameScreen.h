#pragma once

#include "../../Maths.h"
#include "../Input.h"
#include "../Renderer/Framebuffer.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../Renderer/VertexArray.h"
#include "Screen.h"
#include <SFML/System/Clock.hpp>

#include "../Camera.h"

class InGameScreen final : public Screen {
  public:
    InGameScreen(ScreenManager& stack);
    ~InGameScreen();

    void onInput(const sf::Window& window, const Keyboard& keyboard) override;
    void onUpdate(float dt) override;
    void onRender(Framebuffer&) override;

  private:
    void renderScene(const glm::vec4& clippingPlane);

    bool m_isPaused = false;
    bool m_isSettingsOpened = false;
    void showPauseMenu();

    FirstPersonController m_firstPersonController;

    sf::Clock m_timer;

    Framebuffer m_reflection;
    Framebuffer m_refraction;
    Texture2d m_waterTexture;
    // Texture2d m_waterNormalTexture;
    // Texture2d m_waterDisplaceTexture;

    VertexArray m_cubeVao;

    Shader m_shader;
    Shader m_waterShader;

    Shader m_guiDebugShader;
    VertexArray m_guiDebugQuad;

    std::vector<Terrain> m_terrains;
    std::vector<Terrain> m_waters;

    std::vector<std::pair<glm::vec3, glm::vec3>> m_cubePositions;

    ViewFrustum m_frustum;
};