#include "ScenarioOverlay.hpp"

#include "UI/Label.hpp"
#include "UI/UIManager.hpp"

#include <format>

using namespace UI;

ScenarioOverlay::ScenarioOverlay()
    : m_screen(UIManager::instance().addScreen())
{
    auto* mainContainer = m_screen->add<Widget>(new Widget({ .x = 0.0, .y = 0.0, .w = 0.2, .h = 0.2 }));
    mainContainer->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    m_challenge = mainContainer->add<Label>(new Label({ .x = 0.1, .y = 0.07, .w = 0.8, .h = 0.1 }));
    m_challenge->setText("Challenge: No", 18);
    m_challenge->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    m_timeRemaining = mainContainer->add<Label>(new Label({ .x = 0.1, .y = 0.2, .w = 0.8, .h = 0.1 }));
    m_timeRemaining->setText("Time remaining: N/A", 18);
    m_timeRemaining->backgroundColor = { .r = 45, .g = 45, .b = 45 };
}

void ScenarioOverlay::update(const ScenarioOverlayData& data)
{
    if (m_lastData.challenge != data.challenge) {
        m_challenge->setText(std::format("Challenge: {}", data.challenge ? "Yes" : "No").c_str(), 18);
    }

    // TODO: This check is kinda useless right now because the float will always be different, even though
    // we only display 2 decimal places
    if (m_lastData.timeRemaining != data.timeRemaining) {
        std::string timeRemaining = std::format("{:.2f}", data.timeRemaining);
        m_timeRemaining->setText(std::format("Time remaining: {}", data.challenge ? timeRemaining : "N/A").c_str(), 18);
    }

    m_lastData = data;
}
