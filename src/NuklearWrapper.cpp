#include "NuklearWrapper.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <algorithm>
#include <array>
#include <iostream>

#define NK_IMPLEMENTATION
#define NK_GLFW_GL4_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>
#include <nuklear_glfw_gl4.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

NuklearWrapper::NuklearWrapper(GLFWwindow* window)
    : m_window(window)
    , m_ctx(nk_glfw3_init(
          m_window, NK_GLFW3_DEFAULT, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER))
{
    glfwGetWindowSize(m_window, &m_width, &m_height);
    struct nk_font_atlas* atlas = nullptr;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();

    const int texWidth = 256;
    const int texHeight = 256;
    std::array<char, (size_t)texWidth * texHeight * 4> pixels;
    std::fill(pixels.begin(), pixels.end(), 128);
    int texIndex = nk_glfw3_create_texture(pixels.data(), texWidth, texHeight);
    m_img = nk_image_id(texIndex);

    m_settings.crosshairColor.r = 1.0f;
    m_settings.crosshairColor.g = 0.0f;
    m_settings.crosshairColor.b = 0.0f;
    m_settings.crosshairColor.a = 1.0f;

    m_settings.sensitivity = 0.03f; // TODO: receive default settings
}

void NuklearWrapper::renderPauseMenu()
{
    nk_glfw3_set_callbacks();
    nk_glfw3_new_frame();

    glfwGetWindowSize(m_window, &m_width, &m_height);
    int rectSize = 300;

    if (nk_begin(m_ctx, "Settings",
            nk_rect((m_width - rectSize) / 2, (m_height - rectSize) / 2,
                rectSize, rectSize),
            NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(m_ctx, 25, 1);
        nk_property_float(
            m_ctx, "Sensitivity:", 0, &m_settings.sensitivity, 1, 0.001, 0.001);

        nk_layout_row_dynamic(m_ctx, 20, 1);
        nk_label(m_ctx, "Crosshair color:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(m_ctx, 25, 1);
        if (nk_combo_begin_color(m_ctx, nk_rgb_cf(m_settings.crosshairColor),
                nk_vec2(nk_widget_width(m_ctx), 400))) {
            nk_layout_row_dynamic(m_ctx, 120, 1);
            m_settings.crosshairColor
                = nk_color_picker(m_ctx, m_settings.crosshairColor, NK_RGBA);
            nk_layout_row_dynamic(m_ctx, 25, 1);
            m_settings.crosshairColor.r = nk_propertyf(m_ctx, "#R:", 0,
                m_settings.crosshairColor.r, 1.0f, 0.01f, 0.005f);
            m_settings.crosshairColor.g = nk_propertyf(m_ctx, "#G:", 0,
                m_settings.crosshairColor.g, 1.0f, 0.01f, 0.005f);
            m_settings.crosshairColor.b = nk_propertyf(m_ctx, "#B:", 0,
                m_settings.crosshairColor.b, 1.0f, 0.01f, 0.005f);
            m_settings.crosshairColor.a = nk_propertyf(m_ctx, "#A:", 0,
                m_settings.crosshairColor.a, 1.0f, 0.01f, 0.005f);
            nk_combo_end(m_ctx);
        }
    }
    nk_end(m_ctx);

    nk_glfw3_render(NK_ANTI_ALIASING_ON);
}

const SettingsData& NuklearWrapper::settings() const
{
    return m_settings;
}