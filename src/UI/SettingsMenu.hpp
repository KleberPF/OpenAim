#pragma once

#include "UI/Screen.hpp"

namespace UI {

class SettingsMenu {
public:
    SettingsMenu(float sensitivity);

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
    // TODO: ideally we would have a SettingsManager or something
    float m_currentSensitivity;

    static void submit(float sensitivity);
};

} // namespace UI
