#pragma once

#include "Renderer.hpp"
#include "UI/Screen.hpp"

#include <memory>
#include <vector>

namespace UI {

class UIManager {
public:
    static void init(float viewWidth, float viewHeight);
    static void shutdown();

    static UIManager& instance(); // maybe const this?

    void subscribe();

    Screen* addScreen();

    void render(Renderer& renderer);

    float viewWidth() const;
    float viewHeight() const;

private:
    UIManager() = default;

    void handleResize(int width, int height);
    void handleMouseButton(int key, bool pressed);
    void handleCursorPos(double xpos, double ypos);

    void onEvent(EventType type, void* data);

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
