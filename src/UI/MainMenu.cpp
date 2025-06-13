#include "MainMenu.hpp"

#include "EventManager.hpp"
#include "Events.hpp"
#include "UI/Button.hpp"
#include "UI/Dropdown.hpp"
#include "UI/Label.hpp"
#include "UI/UIManager.hpp"

using namespace UI;

MainMenu::MainMenu()
    : m_screen(UIManager::instance().addScreen())
{
    m_screen->active = true;

    // Main container
    auto* mainContainer = m_screen->add<Widget>(new Widget({ .x = 0.333, .y = 0.1, .w = 0.333, .h = 0.5 }));
    mainContainer->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    // Title card
    auto* titleCard = mainContainer->add<Label>(new Label({ .x = 0.1, .y = 0.07, .w = 0.8, .h = 0.1 }));
    titleCard->setText("OpenAim");
    titleCard->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    // Dropdown
    auto* dropdown = mainContainer->add(new Dropdown({ .x = 0.1, .y = 0.21, .w = 0.8, .h = 0.1 }));
    dropdown->backgroundColor = { .r = 50, .g = 50, .b = 50 };
    dropdown->addOption("Clicking");
    dropdown->addOption("Tracking");
    dropdown->addOption("Switching");

    // Buttons
    auto* challengeBtn = mainContainer->add<Button>(new Button({ .x = 0.2, .y = 0.7, .w = 0.6, .h = 0.1 }));
    challengeBtn->setText("Challenge");
    challengeBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    challengeBtn->onClick = [dropdown]() {
        MainMenu::submit(dropdown->selectedOption(), true);
    };

    // auto* freePlayBtn = m_screen->add<Button>(new Button({ .x = 0.3995, .y = 0.51, .w = 0.2, .h = 0.05 }));
    // freePlayBtn->setText("Free Play");
    // freePlayBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    // freePlayBtn->onClick = [dropdown]() {
    //     MainMenu::submit(dropdown->selectedOption(), false);
    // };
}

void MainMenu::submit(const std::string& scenarioName, bool challenge)
{
    auto event = NewScenarioEvent {
        .scenario = scenarioName,
        .challenge = challenge
    };
    EventManager::instance().triggerEvent(EventType::StartScenario, &event);
}
