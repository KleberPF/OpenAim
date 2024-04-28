#pragma once

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
};

class NuklearWrapper {
public:
    NuklearWrapper(GLFWwindow* window);

    void renderPauseMenu();
    const SettingsData& settings() const;

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    SettingsData m_settings;

    struct nk_context* m_ctx;
    struct nk_colorf m_bg;
    struct nk_image m_img;
};