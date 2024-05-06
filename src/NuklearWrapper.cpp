#include "NuklearWrapper.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstring>
#include <format>
#include <optional>
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
#define MAX_FIELD_SIZE 32

// this whole function is bad...
// interfacing with C strings is annoying
SettingsData::SettingsData(const InternalSettingsData& data)
{
    assert(data.sensitivity.string.len < MAX_FIELD_SIZE - 1);
    assert(data.maxFps.string.len < MAX_FIELD_SIZE - 1);

    char* endPtr = nullptr;
    char buf[MAX_FIELD_SIZE] = { '\0' };
    std::strncpy(buf, nk_str_get_const(&data.sensitivity.string),
        data.sensitivity.string.len);
    float value = std::strtof(buf, &endPtr);
    if (endPtr - buf == data.sensitivity.string.len) { // no error
        sensitivity = value;
    } else {
        sensitivity = std::nullopt;
    }

    std::memset(buf, 0, MAX_FIELD_SIZE);
    std::strncpy(
        buf, nk_str_get_const(&data.maxFps.string), data.maxFps.string.len);
    value = std::strtof(buf, &endPtr);
    if (endPtr - buf == data.maxFps.string.len) { // no error
        maxFps = value;
    } else {
        maxFps = std::nullopt;
    }

    crosshairColor.r = data.crosshairColor.r;
    crosshairColor.g = data.crosshairColor.g;
    crosshairColor.b = data.crosshairColor.b;

    targetColor.r = data.targetColor.r;
    targetColor.g = data.targetColor.g;
    targetColor.b = data.targetColor.b;
}

NuklearWrapper::NuklearWrapper(GLFWwindow* window)
    : m_window(window)
    , m_ctx(nk_glfw3_init(
          m_window, NK_GLFW3_DEFAULT, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER))
{
    glfwGetWindowSize(m_window, &m_width, &m_height);
    struct nk_font_atlas* atlas = nullptr;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();

    // TODO: receive default settings
    m_unsavedSettings.crosshairColor.r = 0.0f;
    m_unsavedSettings.crosshairColor.g = 1.0f;
    m_unsavedSettings.crosshairColor.b = 0.0f;
    m_unsavedSettings.crosshairColor.a = 1.0f;

    m_unsavedSettings.targetColor.r = 0.125f;
    m_unsavedSettings.targetColor.g = 0.55f;
    m_unsavedSettings.targetColor.b = 0.9f;
    m_unsavedSettings.targetColor.a = 1.0f;

    nk_textedit_init_default(&m_unsavedSettings.sensitivity);
    nk_str_append_str_char(&m_unsavedSettings.sensitivity.string, "2.5");
    nk_textedit_init_default(&m_unsavedSettings.maxFps);
    nk_str_append_str_char(&m_unsavedSettings.maxFps.string, "300");
}

void NuklearWrapper::renderBegin()
{
    nk_glfw3_new_frame();
}

std::optional<size_t> NuklearWrapper::renderMainMenu(
    const std::vector<Scenario>& scenarios)
{
    nk_glfw3_set_callbacks();

    glfwGetWindowSize(m_window, &m_width, &m_height);
    int rectSize = 300;
    std::optional<size_t> selectedOption = std::nullopt;

    if (nk_begin(m_ctx, "OpenAim",
            nk_rect((m_width - rectSize) / 2, (m_height - rectSize) / 2,
                rectSize, rectSize),
            NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(m_ctx, 30, 1);
        nk_label(m_ctx, "Select the scenenario:", NK_TEXT_CENTERED);

        std::vector<const char*> scenariosNames;
        scenariosNames.reserve(scenarios.size());
        for (const auto& scenario : scenarios) {
            scenariosNames.push_back(scenario.name.c_str());
        }
        nk_layout_row_dynamic(m_ctx, 20, 1);
        nk_combobox(m_ctx, scenariosNames.data(), scenariosNames.size(),
            &m_selectedScenario, 20, nk_vec2(200, 200));

        if (nk_button_label(m_ctx, "Play")) {
            selectedOption = m_selectedScenario;
        }
    }
    nk_end(m_ctx);

    return selectedOption;
}

std::optional<SettingsData> NuklearWrapper::renderPauseMenu()
{
    nk_glfw3_set_callbacks();

    glfwGetWindowSize(m_window, &m_width, &m_height);
    int rectSize = 350;
    std::optional<SettingsData> result = std::nullopt;

    if (nk_begin(m_ctx, "Settings",
            nk_rect((m_width - rectSize) / 2 + 200, (m_height - rectSize) / 2,
                rectSize, rectSize),
            NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        renderNumberTextField(
            "Max FPS (0 for uncapped): ", m_unsavedSettings.maxFps);
        renderNumberTextField("Sensitivity:", m_unsavedSettings.sensitivity);
        renderColorPicker("Crosshair color:", m_unsavedSettings.crosshairColor);
        renderColorPicker("Target color:", m_unsavedSettings.targetColor);

        if (nk_button_label(m_ctx, "Save")) {
            result = SettingsData(m_unsavedSettings);
        }
    }
    nk_end(m_ctx);

    return result;
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

void NuklearWrapper::renderNumberTextField(
    const std::string& label, nk_text_edit& edit)
{
    nk_layout_row_begin(m_ctx, NK_DYNAMIC, 20, 2);
    nk_layout_row_push(m_ctx, 0.75f);
    nk_label(m_ctx, label.c_str(), NK_TEXT_ALIGN_LEFT);
    nk_layout_row_push(m_ctx, 0.25f);
    nk_edit_buffer(
        m_ctx, NK_EDIT_FIELD, &edit, NuklearWrapper::numbersOnlyFilter);
    nk_layout_row_end(m_ctx);
}

// This is probably easy to break
nk_bool NuklearWrapper::numbersOnlyFilter(
    const nk_text_edit* edit, nk_rune unicode)
{
    const char* text = nk_str_get_const(&edit->string);
    int len = edit->string.len;

    if (len == 0 && unicode == '.') {
        return false;
    }

    bool hasDot = false;
    for (int i = 0; i < len; i++) {
        if (text[i] == '.') {
            hasDot = true;
            break;
        }
    }

    return edit->string.len <= 3
        && (isdigit(unicode) || (!hasDot && unicode == '.'));
}
