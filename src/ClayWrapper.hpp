#pragma once

#include "EventManager.hpp"

#include <clay.h>

#include <cstdint>
#include <map>
#include <vector>

struct ClayRenderData {
    Clay_RenderCommandArray renderCommands;
    float viewWidth;
    float viewHeight;
};

struct ClayElementState {
    bool clicked = false;
    bool wasPressedLastFrame = false;
};

class ClayWrapper {
public:
    ClayWrapper(float screenWidth, float screenHeight);

    ClayRenderData buildMainMenu();

    void subscribe(EventManager& eventManager);

private:
    void handleResize(int width, int height);
    void handleMouseButton(int key, bool pressed);
    void handleCursorPos(double xpos, double ypos);

    void scenarioButton(Clay_String id, Clay_String label);

    std::vector<uint8_t> m_clayBuffer;
    std::map<uint32_t, ClayElementState> m_uiState;

    float m_viewWidth;
    float m_viewHeight;

    struct {
        float x;
        float y;
    } m_cursorPos;
    bool m_mouseDown; // TODO: check how clay handles different mouse buttons
};
