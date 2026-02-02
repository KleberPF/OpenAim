#include "ScenarioOverlay.hpp"

#include "UI/Label.hpp"
#include "UI/UIManager.hpp"

#include <format>

using namespace UI;

ScenarioOverlay::ScenarioOverlay(const ScenarioOverlayData& data)
    : m_screen(UIManager::instance().addScreen())
{
    auto* mainContainer = m_screen->add<Widget>(new Widget({ .x = 0.0, .y = 0.0, .w = 0.2, .h = 0.2 }));
    mainContainer->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    m_challenge = mainContainer->add<Label>(new Label({ .x = 0.1, .y = 0.07, .w = 0.8, .h = 0.1 }));
    m_challenge->setText("Challenge: No");
    m_challenge->backgroundColor = { .r = 00, .g = 45, .b = 45 };
}

void ScenarioOverlay::update(const ScenarioOverlayData& data)
{
    if (m_lastData.challenge != data.challenge) {
        m_challenge->setText(std::format("Challenge: {}", data.challenge ? "Yes" : "No").c_str());
    }
}
