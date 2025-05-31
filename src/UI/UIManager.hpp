#pragma once

#include "Renderer.hpp"
#include "UI/Screen.hpp"

#include <memory>
#include <vector>

namespace UI {

class UIManager {
public:
    UIManager(float viewWidth, float viewHeight);

    void subscribe();

    Screen* addScreen();

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

    std::vector<std::unique_ptr<Screen>> m_screens;
    friend class Screen;
};

} // namespace UI
