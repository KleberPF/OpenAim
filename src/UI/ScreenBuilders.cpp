#include "ScreenBuilders.hpp"

#include "UI/Button.hpp"
#include "UI/Dropdown.hpp"
#include "UI/Label.hpp"
#include "UI/UIManager.hpp"
#include "UI/Widget.hpp"

#include <iostream>

using namespace UI;

Screen* buildMenuScreen(UIManager& uiManager)
{
    Screen* screen = uiManager.addScreen();
    screen->active = true;

    // Main container
    auto* mainContainer = screen->add<Widget>(new Widget({ .x = 0.333, .y = 0.1, .w = 0.333, .h = 0.5 }));
    mainContainer->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    // Title card
    auto* titleCard = screen->add<Label>(new Label({ .x = 0.3995, .y = 0.13, .w = 0.2, .h = 0.05 }));
    titleCard->setText("OpenAim");
    titleCard->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    // Dropdown
    auto* dropdown = screen->add(new Dropdown({ .x = 0.3995, .y = 0.2, .w = 0.2, .h = 0.05 }));
    dropdown->backgroundColor = { .r = 50, .g = 50, .b = 50 };
    dropdown->addOption("Clicking");
    dropdown->addOption("Tracking");
    dropdown->addOption("Switching");

    // Buttons
    auto* challengeBtn = screen->add<Button>(new Button({ .x = 0.3995, .y = 0.45, .w = 0.2, .h = 0.05 }));
    challengeBtn->setText("Challenge");
    challengeBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    challengeBtn->onClick = []() {
        std::cout << "Challenge selected\n";
    };

    auto* freePlayBtn = screen->add<Button>(new Button({ .x = 0.3995, .y = 0.51, .w = 0.2, .h = 0.05 }));
    freePlayBtn->setText("Free Play");
    freePlayBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    freePlayBtn->onClick = []() {
        std::cout << "Free Play selected\n";
    };

    return screen;
}
