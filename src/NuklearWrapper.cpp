#include "NuklearWrapper.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <string>

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

    m_settings.crosshairColor.r = 0.0f;
    m_settings.crosshairColor.g = 1.0f;
    m_settings.crosshairColor.b = 0.0f;
    m_settings.crosshairColor.a = 1.0f;

    m_settings.targetColor.r = 0.125f;
    m_settings.targetColor.g = 0.55f;
    m_settings.targetColor.b = 0.9f;
    m_settings.targetColor.a = 1.0f;

    m_settings.sensitivity = 2.5f; // TODO: receive default settings
    m_unsavedSettings = m_settings;
}

void NuklearWrapper::renderBegin()
{
    nk_glfw3_new_frame();
}

void NuklearWrapper::renderPauseMenu()
{
    nk_glfw3_set_callbacks();

    glfwGetWindowSize(m_window, &m_width, &m_height);
    int rectSize = 300;

    if (nk_begin(m_ctx, "Settings",
            nk_rect((m_width - rectSize) / 2 + 200, (m_height - rectSize) / 2,
                rectSize, rectSize),
            NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(m_ctx, 30, 1);
        nk_label_wrap(m_ctx,
            "For now this number doesn't mean anything so play around with "
            "it");
        nk_layout_row_dynamic(m_ctx, 20, 1);
        nk_property_float(m_ctx, "Sensitivity:", 0,
            &m_unsavedSettings.sensitivity, 30, 0.1, 0.1);

        renderColorPicker("Crosshair color:", m_unsavedSettings.crosshairColor);
        renderColorPicker("Target color:", m_unsavedSettings.targetColor);

        if (nk_button_label(m_ctx, "Save")) {
            m_settings = m_unsavedSettings;
        }
    }
    nk_end(m_ctx);
}

void NuklearWrapper::renderStats(
    int shotsHit, int totalShots, float timeElapsedSeconds, float fps)
{
    if (nk_begin(m_ctx, "Stats", nk_rect(10, 10, 200, 180),
            NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
        std::string shotsHitLabel = std::format("Shots hit: {}", shotsHit);
        std::string totalShotsLabel
            = std::format("Total shots: {}", totalShots);
        float accuracy
            = totalShots > 0 ? (float)shotsHit / totalShots * 100 : 0;
        std::string accuracyLabel = std::format("Accuracy: {:.2f}%", accuracy);
        std::string timeElapsedLabel
            = std::format("Time elapsed: {:.2f}s", timeElapsedSeconds);

        std::array<std::string, 5> statsLabels = {
            std::format("FPS: {}", fps),
            std::format("Shots hit: {}", shotsHit),
            std::format("Total shots: {}", totalShots),
            std::format("Accuracy: {:.2f}%", accuracy),
            std::format("Time elapsed: {:.2f}s", timeElapsedSeconds),
        };

        for (auto& label : statsLabels) {
            nk_layout_row_dynamic(m_ctx, 20, 1);
            nk_label(m_ctx, label.c_str(), NK_TEXT_LEFT);
        }
    }

    nk_end(m_ctx);
}

void NuklearWrapper::renderEnd()
{
    nk_glfw3_render(NK_ANTI_ALIASING_ON);
}

const SettingsData& NuklearWrapper::settings() const
{
    return m_settings;
}

void NuklearWrapper::renderColorPicker(
    const std::string& name, nk_colorf& color)
{
    nk_layout_row_dynamic(m_ctx, 20, 1);
    nk_label(m_ctx, name.c_str(), NK_TEXT_LEFT);
    nk_layout_row_dynamic(m_ctx, 25, 1);
    if (nk_combo_begin_color(
            m_ctx, nk_rgb_cf(color), nk_vec2(nk_widget_width(m_ctx), 400))) {
        nk_layout_row_dynamic(m_ctx, 120, 1);
        color = nk_color_picker(m_ctx, color, NK_RGBA);
        nk_layout_row_dynamic(m_ctx, 25, 1);
        color.r = nk_propertyf(m_ctx, "#R:", 0, color.r, 1.0f, 0.01f, 0.005f);
        color.g = nk_propertyf(m_ctx, "#G:", 0, color.g, 1.0f, 0.01f, 0.005f);
        color.b = nk_propertyf(m_ctx, "#B:", 0, color.b, 1.0f, 0.01f, 0.005f);
        color.a = nk_propertyf(m_ctx, "#A:", 0, color.a, 1.0f, 0.01f, 0.005f);
        nk_combo_end(m_ctx);
    }
}
