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
    void handleMouseButton(int key, bool pressed);
    void handleCursorPos(double xpos, double ypos);

    std::vector<uint8_t> m_clayBuffer;

    float m_viewWidth;
    float m_viewHeight;

    struct {
        float x;
        float y;
    } m_cursorPos;
    bool m_mouseDown; // TODO: check how clay handles different mouse buttons
};
