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

    // Buttons
    // auto* clickingBtn = screen->add<Button>(new Button({ .x = 0.3995, .y = 0.2, .w = 0.2, .h = 0.05 }));
    // clickingBtn->setText("Clicking");
    // clickingBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    // clickingBtn->onClick = []() {
    //     std::cout << "Clicking selected\n";
    // };

    // auto* trackingBtn = screen->add<Button>(new Button({ .x = 0.3995, .y = 0.26, .w = 0.2, .h = 0.05 }));
    // trackingBtn->setText("Tracking");
    // trackingBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    // trackingBtn->onClick = []() {
    //     std::cout << "Tracking selected\n";
    // };

    // auto* switchingBtn = screen->add<Button>(new Button({ .x = 0.3995, .y = 0.32, .w = 0.2, .h = 0.05 }));
    // switchingBtn->setText("Switching");
    // switchingBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    // switchingBtn->onClick = []() {
    //     std::cout << "Switching selected\n";
    // };

    auto* dropdown = screen->add(new Dropdown({ .x = 0.3995, .y = 0.2, .w = 0.2, .h = 0.05 }));
    dropdown->addOption("Clicking");
    dropdown->addOption("Tracking");
    dropdown->addOption("Switching");

    return screen;
}
