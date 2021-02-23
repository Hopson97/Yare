#include "ClientSettings.h"
#include "Screen/Screen.h"
#include <fstream>
#include <imgui/imgui.h>

ClientSettings::ClientSettings()
{
    std::ifstream inFile("Data/settings");
    std::string val;
    while (inFile >> val) {
        // clang-format off
        if      (val == "show_fps") inFile >> showFps;
        else if (val == "bloom")    inFile >> useBloomShaders;

        // clang-format on
    }
}

ClientSettings::~ClientSettings()
{
    // clang-format off
    std::ofstream outFile("Data/settings");
    outFile << "show_fps "  << showFps           << '\n';
    outFile << "bloom "     << useBloomShaders   << '\n';
    // clang-format on
}

void ClientSettings::showSettingsMenu(std::function<void(void)> onBackPressed)
{
    if (imguiBeginCustom("S E T T I N G S")) {
        ImGui::Separator();
        ImGui::Text("Basic Settings");
        ImGui::Checkbox("Show FPS Counter", &showFps);
        ImGui::Checkbox("Enable Bloom", &useBloomShaders);

        ImGui::Separator();

        ImGui::Separator();
        if (imguiButtonCustom("Back")) {
            onBackPressed();
        }
    }
    ImGui::End();
}
