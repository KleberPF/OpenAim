#pragma once

#include "UI/Screen.hpp"
#include <string>
namespace UI {

class MainMenu {
public:
    MainMenu(UIManager& uiManager);

private:
    Screen* m_screen;

    void submit(const std::string& scenarioName, bool challenge);
};

} // namespace UI