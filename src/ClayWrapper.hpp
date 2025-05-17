#pragma once

#include "EventManager.hpp"

#include <clay.h>

#include <cstdint>
#include <vector>

struct ClayRenderData {
    Clay_RenderCommandArray renderCommands;
    float viewWidth;
    float viewHeight;
};

class ClayWrapper {
public:
    ClayWrapper(float screenWidth, float screenHeight);

    ClayRenderData buildTestUi();

    void subscribe(EventManager& eventManager);

private:
    void handleResize(int width, int height);
    void handleKey(int key, bool pressed);
    void handleMouseButton(int key, bool pressed);
    void handleCursorPos(double xpos, double ypos);

    std::vector<uint8_t> m_clayBuffer;

    float m_viewWidth;
    float m_viewHeight;
};
