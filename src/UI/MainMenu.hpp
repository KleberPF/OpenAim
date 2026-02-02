#pragma once

#include "UI/Screen.hpp"

#include <string>

namespace UI {

class MainMenu {
public:
    MainMenu(const std::vector<std::string>& scenarios);

private:
    Screen* m_screen;

    static void submit(const std::string& scenarioName, bool challenge);
};

} // namespace UI
