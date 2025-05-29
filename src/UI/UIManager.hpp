#pragma once

#include "EventManager.hpp"
#include "Renderer.hpp"
#include "UI/Screen.hpp"

#include <vector>

namespace UI {

class UIManager {
public:
    UIManager(float viewWidth, float viewHeight);

    void subscribe(EventManager& eventManager);

    void onFrame();
    void addScreen(Screen screen);

    void render(Renderer& renderer);

private:
    void handleResize(int width, int height);
    void handleMouseButton(int key, bool pressed);
    void handleCursorPos(double xpos, double ypos);

    float m_viewWidth;
    float m_viewHeight;

    struct {
        float x;
        float y;
    } m_cursorPos;

    std::vector<Screen> m_screens;
};

} // namespace UI
