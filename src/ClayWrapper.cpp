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
{
    uint64_t totalMemorySize = Clay_MinMemorySize();
    m_clayBuffer.reserve(totalMemorySize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(
        totalMemorySize, m_clayBuffer.data());

    Clay_Initialize(arena, (Clay_Dimensions) { screenWidth, screenHeight },
        (Clay_ErrorHandler) { handleClayErrors, nullptr });
}

Clay_RenderCommandArray ClayWrapper::buildTestUi()
{
    const Clay_Color COLOR_RED = (Clay_Color) { 168, 66, 28, 255 };
    const Clay_Color COLOR_LIGHT = (Clay_Color) { 224, 215, 210, 255 };

    Clay_BeginLayout();
    // clang-format off
    CLAY({
        .id = CLAY_ID("OuterContainer"),
        .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 },
        .backgroundColor = {250, 250, 255, 255} })
    {
        CLAY({
            .id = CLAY_ID("SideBar"),
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16, .layoutDirection = CLAY_TOP_TO_BOTTOM },
            .backgroundColor = COLOR_LIGHT
        }) { }
    }
    // clang-format on
    return Clay_EndLayout();
}
