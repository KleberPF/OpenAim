#include "ClayWrapper.hpp"

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <iostream>

namespace {

void handleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    std::cout << errorData.errorText.chars << '\n';
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
}

ClayRenderData ClayWrapper::buildTestUi()
{
    const Clay_Color COLOR_RED = (Clay_Color) { 168, 66, 28, 255 };
    const Clay_Color COLOR_LIGHT = (Clay_Color) { 224, 215, 210, 255 };

    Clay_SetLayoutDimensions((Clay_Dimensions) { m_viewWidth, m_viewHeight });

    Clay_BeginLayout();
    // clang-format off
    CLAY({
        .id = CLAY_ID("OuterContainer"),
        .layout = { .sizing = {.width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(48)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 },
        .backgroundColor = {255, 0, 0, 255} })
    {
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
    eventManager.addKeyListener([this](int key, bool pressed) {
        handleKey(key, pressed);
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

void ClayWrapper::handleKey(int key, bool pressed)
{
}

void ClayWrapper::handleMouseButton(int key, bool pressed)
{
}

void ClayWrapper::handleCursorPos(double xpos, double ypos)
{
}
