#include "ScreenBuilders.hpp"

#include "UI/UIManager.hpp"

#include <iostream>

Screen* buildMenuScreen(UIManager& uiManager)
{
    UI::Screen* screen = uiManager.addScreen();
    screen->active = true;

    // Main container
    UI::Widget* mainContainer = screen->addWidget({ .x = 0.333, .y = 0.1, .w = 0.333, .h = 0.5 });
    mainContainer->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    // Title card
    UI::Widget* titleCard = screen->addWidget({ .x = 0.3995, .y = 0.13, .w = 0.2, .h = 0.05 });
    titleCard->setText("OpenAim");
    titleCard->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    // Buttons
    UI::Widget* clickingBtn = screen->addWidget({ .x = 0.3995, .y = 0.2, .w = 0.2, .h = 0.05 });
    clickingBtn->setText("Clicking");
    clickingBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    clickingBtn->onClick = []() {
        std::cout << "Clicking selected\n";
    };

    UI::Widget* trackingBtn = screen->addWidget({ .x = 0.3995, .y = 0.26, .w = 0.2, .h = 0.05 });
    trackingBtn->setText("Tracking");
    trackingBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    trackingBtn->onClick = []() {
        std::cout << "Tracking selected\n";
    };

    UI::Widget* switchingBtn = screen->addWidget({ .x = 0.3995, .y = 0.32, .w = 0.2, .h = 0.05 });
    switchingBtn->setText("Switching");
    switchingBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    switchingBtn->onClick = []() {
        std::cout << "Switching selected\n";
    };

    return screen;
}
