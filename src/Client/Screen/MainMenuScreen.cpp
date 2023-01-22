#include "MainMenuScreen.h"

#include "../ClientSettings.h"
#include "InGameScreen.h"
#include <imgui/imgui.h>
#include <iostream>

MainMenuScreen::MainMenuScreen(ScreenManager& stack)
    : Screen(stack)

{
}

void MainMenuScreen::onRender(Framebuffer&)
{
    switch (m_activeMenu) {
        case Menu::MainMenuScreen:
            if (imguiBeginCustom("M A I N   M E N U")) {
                ImGui::Text("Y A R E");
                ImGui::Separator();
                if (imguiButtonCustom("Start Game")) {
                    m_pScreens->pushScreen(std::make_unique<InGameScreen>(*m_pScreens));
                }
                if (imguiButtonCustom("Settings")) {
                    m_activeMenu = Menu::SettingsMenu;
                }
                if (imguiButtonCustom("Exit Game")) {
                    m_pScreens->popScreen();
                }
            }
            ImGui::End();
            break;

        case Menu::SettingsMenu:
            ClientSettings::get().showSettingsMenu(
                [&] { m_activeMenu = Menu::MainMenuScreen; });
            break;
    }
}