#pragma once

#include "UI/Screen.hpp"

#include <string>

namespace UI {

class MainMenu {
public:
    MainMenu(const std::vector<std::string>& scenarios);

    void enable()
    {
        m_screen->active = true;
    }
    void disable()
    {
        m_screen->active = false;
    }

private:
    Screen* m_screen;

    static void submit(const std::string& scenarioName, bool challenge);
};

} // namespace UI
