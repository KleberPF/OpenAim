#pragma once

#include "UI/Label.hpp"
#include "UI/Screen.hpp"

namespace UI {

struct ScenarioOverlayData {
    bool challenge = false;
};

class ScenarioOverlay {
public:
    ScenarioOverlay(const ScenarioOverlayData& data);

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
};

} // namespace UI
