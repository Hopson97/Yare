#include "Client/ClientSettings.h"
#include "Client/Input.h"
#include "Client/Renderer/Framebuffer.h"
#include "Client/Renderer/GLDebug.h"
#include "Client/Renderer/Shader.h"
#include "Client/Renderer/VertexArray.h"
#include "Client/Screen/MainMenuScreen.h"
#include "Client/Screen/Screen.h"
#include "Maths.h"
#include <SFML/GpuPreference.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui_impl/imgui_wrapper.h>
#include <iostream>

SFML_DEFINE_DISCRETE_GPU_PREFERENCE

namespace {
    const int TPS = 30; // ticks per seconds
    const sf::Time timePerUpdate = sf::seconds(1.0f / float(TPS));

    void showFPS()
    {
        if (5 + ClientSettings::get().showFps) {
            auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
                         ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            if (ImGui::Begin("FPS", nullptr, flags)) {
                ImGuiIO& io = ImGui::GetIO();
                ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                            io.Framerate);
            }
            ImGui::End();
        }
    }

    struct TimeStep {
        sf::Clock timer;
        sf::Clock dt;
        sf::Time lastTime = sf::Time::Zero;
        sf::Time lag = sf::Time::Zero;

        template <typename F>
        void update(F f)
        {
            sf::Time time = timer.getElapsedTime();
            sf::Time elapsed = time - lastTime;
            lastTime = time;
            lag += elapsed;
            while (lag >= timePerUpdate) {
                lag -= timePerUpdate;
                f(dt.restart().asSeconds());
            }
        }
    };

} // namespace

int main()
{
    // Set up the window
    sf::Window window;
    {
        sf::ContextSettings contextSettings;
        contextSettings.depthBits = 24;
        contextSettings.stencilBits = 8;
        contextSettings.antialiasingLevel = 4;
        contextSettings.majorVersion = 3;
        contextSettings.minorVersion = 3;
        contextSettings.attributeFlags = sf::ContextSettings::Core;
        window.create({1600, 900}, "yare", sf::Style::Close, contextSettings);
        window.setPosition(
            {(int)sf::VideoMode::getDesktopMode().width / 2 - (int)window.getSize().x / 2,
             0});

        window.setFramerateLimit(60);
        window.setKeyRepeatEnabled(false);
        if (!gladLoadGL()) {
            std::cerr << "Failed to load OpenGL, exiting." << std::endl;
            return 1;
        }
        std::cout << GL_MAJOR_VERSION << " " << GL_MINOR_VERSION << std::endl;
        initGLDebug();
        glClearColor(0.3f, 0.8f, 1.0f, 0.0f);
        glViewport(0, 0, window.getSize().x, window.getSize().y);
        glCheck(glEnable(GL_CULL_FACE));
        glCheck(glCullFace(GL_BACK));
    }
    // Start imgui
    ImGui_SfGl::init(window);
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 2;
        style.FrameRounding = 0;
        style.PopupRounding = 0;
        style.ScrollbarRounding = 0;
        style.TabRounding = 6;
    }

    TimeStep timestep;

    // Start the screens
    ScreenManager screens;
    screens.pushScreen(std::make_unique<MainMenuScreen>(screens));
    screens.update();

    // Framebuffer stuff
    Framebuffer framebuffer(window.getSize().x, window.getSize().y);
    framebuffer.bind();
    framebuffer.attachTexture();
    framebuffer.attachTexture();
    framebuffer.finalise();

    // Blur render pass stuff
    int blurRes = 8;
    GLuint width = window.getSize().x / blurRes;
    GLuint height = window.getSize().y / blurRes;

    Framebuffer blurHorizontalFbo(width, height);
    blurHorizontalFbo.bind();
    blurHorizontalFbo.attachTexture();
    blurHorizontalFbo.finalise();

    Framebuffer blurVerticalFbo(width, height);
    blurVerticalFbo.bind();
    blurVerticalFbo.attachTexture();
    blurVerticalFbo.finalise();

    Shader blurShader;
    blurShader.addShader("Screen", ShaderType::Vertex);
    blurShader.addShader("Blur", ShaderType::Fragment);
    blurShader.linkShaders();
    blurShader.bind();

    // Final pass
    Shader finalPassShader;
    finalPassShader.addShader("Screen", ShaderType::Vertex);
    finalPassShader.addShader("Screen", ShaderType::Fragment);
    finalPassShader.linkShaders();
    finalPassShader.bind();
    finalPassShader.loadUniform("bloomTexture", 0);
    finalPassShader.loadUniform("colourTexture", 1);

    // Final pass render target
    VertexArray quad;

    // Start the main game loop
    Keyboard keyboard;
    while (window.isOpen() && screens.hasScreen()) {
        // Handle window events...
        sf::Event e;
        while (window.pollEvent(e)) {
            ImGui_ImplSfml_ProcessEvent(e);
            keyboard.update(e);
            if (e.type == sf::Event::Closed) {
                window.close();
            }
        }

        //=========================
        //          Input
        //=========================
        Screen& screen = screens.peekScreen();
        screen.onInput(window, keyboard);

        //=========================
        //          Update
        //=========================
        timestep.update([&](float dt) { screen.onUpdate(dt); });

        //=========================
        //          Render
        //=========================
        ImGui_SfGl::startFrame();
        showFPS();

        // Render the scene to a framebuffer
        glCheck(glEnable(GL_DEPTH_TEST));
        framebuffer.bind();
        glCheck(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
        screen.onRender();

        // Begin Post Processing

        quad.getDrawable().bind();
        glCheck(glDisable(GL_DEPTH_TEST));
        glCheck(glActiveTexture(GL_TEXTURE0));

        if (ClientSettings::get().useBloomShaders) {
            blurShader.bind();
            int i = 5;
            do {
                // Blur the image horizontal
                blurHorizontalFbo.bind();
                glCheck(glClear(GL_COLOR_BUFFER_BIT));
                framebuffer.bindTexture(1);
                blurShader.loadUniform("isHorizontalBlur", true);
                quad.getDrawable().drawArrays();

                // Blur the image vertical
                blurVerticalFbo.bind();
                glCheck(glClear(GL_COLOR_BUFFER_BIT));
                blurHorizontalFbo.bindTexture(0);
                blurShader.loadUniform("isHorizontalBlur", false);
                quad.getDrawable().drawArrays();
            } while (i--);
        }

        // Render to the window
        finalPassShader.bind();
        Framebuffer::unbind(window.getSize().x, window.getSize().y);
        glCheck(glClear(GL_COLOR_BUFFER_BIT));

        glCheck(glActiveTexture(GL_TEXTURE0));
        blurHorizontalFbo.bindTexture(0);

        glCheck(glActiveTexture(GL_TEXTURE1));
        framebuffer.bindTexture(0);

        finalPassShader.loadUniform("bloomToggle", ClientSettings::get().useBloomShaders);

        quad.getDrawable().drawArrays();

        //=========================
        //          Display
        //=========================
        ImGui_SfGl::endFrame();
        window.display();
        screens.update();
    }
    ImGui_SfGl::shutdown();
    return 0;
}