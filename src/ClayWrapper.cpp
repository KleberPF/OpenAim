#include "ClayWrapper.hpp"

#include "ResourceManager.hpp"
#include "Text.hpp"

#include <cstdint>
#include <optional>
#include <string>

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <iostream>

namespace {

void handleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    std::cout << errorData.errorText.chars << '\n';
}

Clay_Dimensions measureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* /* userData */)
{
    Text t(&ResourceManager::instance().getFont(config->fontId), std::string(text.chars, text.length).c_str(), config->fontSize);
    return (Clay_Dimensions) {
        .width = static_cast<float>(t.width()),
        .height = static_cast<float>(t.height())
    };
}

} // namespace

ClayWrapper::ClayWrapper(float screenWidth, float screenHeight)
    : m_viewWidth(screenWidth)
    , m_viewHeight(screenHeight)
{
    uint64_t totalMemorySize = Clay_MinMemorySize();
    m_clayBuffer.reserve(totalMemorySize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(
        totalMemorySize, m_clayBuffer.data());

    Clay_Initialize(arena, (Clay_Dimensions) { screenWidth, screenHeight },
        (Clay_ErrorHandler) { handleClayErrors, nullptr });
    Clay_SetMeasureTextFunction(measureText, nullptr);
}

MenuData ClayWrapper::buildMainMenu()
{
    Clay_SetLayoutDimensions((Clay_Dimensions) { m_viewWidth, m_viewHeight });
    Clay_SetPointerState((Clay_Vector2) { m_cursorPos.x, m_cursorPos.y }, m_mouseDown);

    MenuData menuData = {
        .renderData = {
            .viewWidth = m_viewWidth,
            .viewHeight = m_viewHeight },
        .scenarioId = std::nullopt
    };

    if (m_uiState[CLAY_ID("ClickingButton").id].clicked) {
        menuData.scenarioId = 0;
    } else if (m_uiState[CLAY_ID("TrackingButton").id].clicked) {
        menuData.scenarioId = 1;
    } else if (m_uiState[CLAY_ID("SwitchingButton").id].clicked) {
        menuData.scenarioId = 2;
    }

    // clang-format off
    Clay_BeginLayout();
    CLAY({
        .id = CLAY_ID("ScreenContainer"),
        .layout = {
            .sizing = {.width = CLAY_SIZING_GROW(800), .height = CLAY_SIZING_GROW(600)},
            .padding = CLAY_PADDING_ALL(16), .childGap = 16,
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER
            }
        },
    })
    {
        CLAY({
            .id = CLAY_ID("MainContainer"),
            .layout = {
                .sizing = {.width = CLAY_SIZING_PERCENT(0.2), .height = CLAY_SIZING_PERCENT(0.4)},
                .padding = CLAY_PADDING_ALL(4), .childGap = 4,
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER,
                    .y = CLAY_ALIGN_Y_TOP
                },
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
            .backgroundColor = {45.0f, 45.0f, 45.0f, 255.0f}
        })
        {
            CLAY({
                .id = CLAY_ID("TitleContainer"),
                .layout = {
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(30)},
                    .padding = CLAY_PADDING_ALL(16), .childGap = 16,
                    .childAlignment = {
                        .x = CLAY_ALIGN_X_CENTER,
                        .y = CLAY_ALIGN_Y_CENTER
                    }
                },
                .backgroundColor = {40.0f, 40.0f, 40.0f, 255.0f}
            })
            {
                CLAY_TEXT(CLAY_STRING("OpenAim"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontId = LIBERATION, .fontSize = 18 }));
            }
            CLAY({
                .id = CLAY_ID("BodyContainer"),
                .layout = {
                    .sizing = {.width =  CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                    .padding = CLAY_PADDING_ALL(16), .childGap = 4,
                    .childAlignment = {
                        .x = CLAY_ALIGN_X_CENTER,
                        .y = CLAY_ALIGN_Y_TOP
                    },
                    .layoutDirection = CLAY_TOP_TO_BOTTOM
                },
                .backgroundColor = {40.0f, 40.0f, 40.0f, 255.0f},
            })
            {
                CLAY_TEXT(CLAY_STRING("Select the scenario:"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontId = LIBERATION, .fontSize = 12 }));
                CLAY({
                    .id = CLAY_ID("ButtonContainer"),
                    .layout = {
                        .sizing = {.width =  CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                        .padding = CLAY_PADDING_ALL(16), .childGap = 4,
                        .childAlignment = {
                            .x = CLAY_ALIGN_X_CENTER,
                            .y = CLAY_ALIGN_Y_TOP
                        },
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    },
                    .backgroundColor = {40.0f, 40.0f, 40.0f, 255.0f},
                })
                {
                    scenarioButton(CLAY_STRING("ClickingButton"), CLAY_STRING("Clicking"));
                    scenarioButton(CLAY_STRING("SwitchingButton"), CLAY_STRING("Switching"));
                    scenarioButton(CLAY_STRING("TrackingButton"), CLAY_STRING("Tracking"));
                }
            }
        }
    }
    // clang-format on

    menuData.renderData.renderCommands = Clay_EndLayout();

    return menuData;
}

void ClayWrapper::subscribe(EventManager& eventManager)
{
    eventManager.addResizeListener([this](int width, int height) {
        handleResize(width, height);
    });
    eventManager.addMouseButtonListener([this](int key, bool pressed) {
        handleMouseButton(key, pressed);
    });
    eventManager.addCursorPosListener([this](double xpos, double ypos) {
        handleCursorPos(xpos, ypos);
    });
}

void ClayWrapper::handleResize(int width, int height)
{
    m_viewWidth = width;
    m_viewHeight = height;
}

void ClayWrapper::handleMouseButton(int /*key*/, bool pressed)
{
    m_mouseDown = pressed;
}

void ClayWrapper::handleCursorPos(double xpos, double ypos)
{
    m_cursorPos.x = xpos;
    m_cursorPos.y = ypos;
}

void ClayWrapper::scenarioButton(Clay_String id, Clay_String label)
{
    // clang-format off
    auto onHover = [](Clay_ElementId elemId, Clay_PointerData data, intptr_t userData) {
        auto* self = (ClayWrapper*)(userData);
        uint32_t id = elemId.id;

        self->m_uiState[id].clicked = false;

        if (self->m_uiState[id].wasPressedLastFrame && data.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
            self->m_uiState[id].clicked = true;
        }

        self->m_uiState[id].wasPressedLastFrame = data.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME || data.state == CLAY_POINTER_DATA_PRESSED;
    };

    CLAY({
        .id = CLAY_SID(id),
        .layout = {
            .sizing = {.width =  CLAY_SIZING_PERCENT(0.4), .height = CLAY_SIZING_FIXED(25)},
            .padding = CLAY_PADDING_ALL(16), .childGap = 16,
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER
            },
        },
        .backgroundColor = {45.0f, 45.0f, 45.0f, 255.0f}
    })
    {
        Clay_OnHover(onHover, (intptr_t)this);
        CLAY_TEXT(label, CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontId = LIBERATION, .fontSize = 12 }));
    }
    // clang-format on
}
