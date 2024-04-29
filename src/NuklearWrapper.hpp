#pragma once

#include <string>
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

struct SettingsData {
    float sensitivity;
    nk_colorf crosshairColor;
    nk_colorf targetColor;
};

class NuklearWrapper {
public:
    NuklearWrapper(GLFWwindow* window);

    static void renderBegin();
    void renderPauseMenu();
    void renderStats(int shotsHit, int totalShots, float timeElapsedSeconds);
    static void renderEnd();

    const SettingsData& settings() const;

private:
    void renderColorPicker(const std::string& name, nk_colorf& color);

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    SettingsData m_settings;
    SettingsData m_unsavedSettings;

    struct nk_context* m_ctx;
};