#pragma once

#include "UI/Label.hpp"
#include "UI/Screen.hpp"

namespace UI {

struct ScenarioOverlayData {
    bool challenge = false;
    float timeRemaining = 0;
    int shotsHit = 0;
    int shotsFired = 0;
};

class ScenarioOverlay {
public:
    ScenarioOverlay();

    void enable()
    {
        m_screen->active = true;
    }
    void disable()
    {
        m_screen->active = false;
    }

    void update(const ScenarioOverlayData& data);

private:
    ScenarioOverlayData m_lastData;

    Screen* m_screen;
    Label* m_challenge;
    Label* m_timeRemaining;
    Label* m_shotsHit;
    Label* m_shotsFired;
};

} // namespace UI
