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
        .layout = { .sizing = {.width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(48)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 },
        .backgroundColor = {255, 0, 0, 255} })
    {
    }
    // clang-format on
    return Clay_EndLayout();
}
