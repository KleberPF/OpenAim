#include "ClayWrapper.hpp"

#include "ResourceManager.hpp"
#include "Text.hpp"

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

ClayRenderData ClayWrapper::buildTestUi()
{
    Clay_SetLayoutDimensions((Clay_Dimensions) { m_viewWidth, m_viewHeight });
    Clay_SetPointerState((Clay_Vector2) { m_cursorPos.x, m_cursorPos.y }, m_mouseDown);

    Clay_BeginLayout();
    // clang-format off
    CLAY({
        .id = CLAY_ID("OuterContainer"),
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
            .id = CLAY_ID("InnerContainer"),
            .layout = {
                .sizing = {.width = CLAY_SIZING_PERCENT(0.3), .height = CLAY_SIZING_PERCENT(0.4)},
                .padding = CLAY_PADDING_ALL(16), .childGap = 16,
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER,
                    .y = CLAY_ALIGN_Y_TOP
                }
            },
            .backgroundColor = {255.0f, 0.0f, 0.0f, 255.0f}
        })
        {
            CLAY_TEXT(CLAY_STRING("Hello World"), CLAY_TEXT_CONFIG({ .textColor = {255, 255, 255, 255}, .fontId = LIBERATION, .fontSize = 24 }));
        }
    }
    // clang-format on

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    return {
        .renderCommands = renderCommands,
        .viewWidth = m_viewWidth,
        .viewHeight = m_viewHeight
    };
}

ClayRenderData ClayWrapper::buildMainMenu()
{
    Clay_SetLayoutDimensions((Clay_Dimensions) { m_viewWidth, m_viewHeight });
    Clay_SetPointerState((Clay_Vector2) { m_cursorPos.x, m_cursorPos.y }, m_mouseDown);

    Clay_BeginLayout();
    // clang-format off
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
                .padding = CLAY_PADDING_ALL(4), .childGap = 16,
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
                    .sizing = {.width = CLAY_SIZING_FIXED(50), .height = CLAY_SIZING_FIXED(50)},
                    .padding = CLAY_PADDING_ALL(16), .childGap = 16,
                    .childAlignment = {
                        .x = CLAY_ALIGN_X_CENTER,
                        .y = CLAY_ALIGN_Y_TOP
                    }
                },
                .backgroundColor = {255.0f, 0.0f, 0.0f, 255.0f}
            })
            {
            }
        }
    }
    // clang-format on

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    return {
        .renderCommands = renderCommands,
        .viewWidth = m_viewWidth,
        .viewHeight = m_viewHeight
    };
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
