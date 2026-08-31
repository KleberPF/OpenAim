#include "SettingsMenu.hpp"

#include "Events.hpp"
#include "UI/Button.hpp"
#include "UI/Label.hpp"
#include "UI/TextArea.hpp"
#include "UI/UIManager.hpp"

using namespace UI;

SettingsMenu::SettingsMenu()
    : m_screen(UIManager::instance().addScreen())
{
    // Main container
    auto* mainContainer = m_screen->add<Widget>(new Widget({ .x = 0.333, .y = 0.1, .w = 0.333, .h = 0.5 }));
    mainContainer->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    auto* sensContainer = mainContainer->add(new Widget({ .x = 0.1, .y = 0.07, .w = 0.8, .h = 0.1 }));
    sensContainer->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    auto* sensLabel = sensContainer->add(new Label({ .x = 0.025, .y = 0.05, .w = 0.7, .h = 0.9 }));
    sensLabel->setText("Sensitivity", 24);
    sensLabel->backgroundColor = { .r = 45, .g = 45, .b = 45 };

    auto* sensTextArea = sensContainer->add(new TextArea({ .x = 0.75, .y = 0.05, .w = 0.2, .h = 0.9 }));
    sensTextArea->setText("", 24);
    sensTextArea->validator = [](const std::string& text) {
        if (text.length() > 5) {
            return false;
        }

        // https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it
        char* end = nullptr;
        double val = strtod(text.c_str(), &end);
        return end != text.c_str() && *end == '\0' && val != HUGE_VAL;
    };

    auto* saveBtn = mainContainer->add<Button>(new Button({ .x = 0.2, .y = 0.7, .w = 0.6, .h = 0.1 }));
    saveBtn->setText("Save");
    saveBtn->backgroundColor = { .r = 40, .g = 40, .b = 40 };
    saveBtn->onClick = [sensTextArea]() {
        float sens = std::stof(sensTextArea->text());
        // TODO: text area has no validation right now
        SettingsMenu::submit(sens);
    };
}

void SettingsMenu::submit(float sensitivity)
{
    auto event = SettingsUpdateEvent {
        .mouseSensitivity = sensitivity
    };
    EventManager::instance().triggerEvent(EventType::SettingsUpdate, &event);
}
