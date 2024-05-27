#pragma once

#include "Scenario.hpp"

#include <optional>
#include <string>
#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include <glm/glm.hpp>
#include <nuklear.h>

struct InternalSettingsData {
    nk_text_edit sensitivity;
    nk_colorf crosshairColor;
    nk_colorf targetColor;
    nk_text_edit maxFps;
};

struct SettingsData {
    SettingsData(const InternalSettingsData& data);

    std::optional<float> sensitivity;
    std::optional<float> maxFps;
    glm::vec3 crosshairColor;
    glm::vec3 targetColor;
};

struct MenuData {
    MenuData(int scenarioOption, bool challenge)
        : scenarioOption(scenarioOption)
        , challenge(challenge)
    {
    }

    int scenarioOption;
    bool challenge;
};

class NuklearWrapper {
public:
    NuklearWrapper(GLFWwindow* window);

    static void renderBegin();
    std::optional<MenuData> renderMainMenu(
        const std::vector<Scenario>& scenarios);
    std::optional<SettingsData> renderPauseMenu();
    void renderStats(
        int shotsHit, int totalShots, float timeElapsedSeconds, float fps);
    void renderChallengeData(
        int shotsHit, int totalShots, float timeRemainingSeconds, float fps);
    bool renderChallengeEndStats(int shotsHit, int totalShots);
    static void renderEnd();

private:
    void renderColorPicker(const std::string& name, nk_colorf& color);
    void renderNumberTextField(const std::string& label, nk_text_edit& edit);

    static nk_bool numbersOnlyFilter(const nk_text_edit*, nk_rune unicode);

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    InternalSettingsData m_unsavedSettings;
    int m_selectedScenario = 0;

    struct nk_context* m_ctx;
};

// Questionable name
struct NuklearRenderScope {
    NuklearRenderScope()
    {
        NuklearWrapper::renderBegin();
    }

    ~NuklearRenderScope()
    {
        NuklearWrapper::renderEnd();
    }
};
