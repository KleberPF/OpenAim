#pragma once

#include "UI/Screen.hpp"

#include <string>

namespace UI {

class SettingsMenu {
public:
    SettingsMenu();

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

    static void submit();
};

} // namespace UI
