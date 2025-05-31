#pragma once

#include <utility>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>
#include <vector>

enum class EventType : uint8_t {
    // GLFW
    Resize,
    KeyPress,
    MouseButton,
    CursorPos,

    // Scenario
    StartScenario,

    Count
};

class EventManager {
public:
    static void init();
    static void shutdown();

    static EventManager& instance(); // maybe const this?

    using EventHandler = std::function<void(EventType,void*)>;
    void addListener(EventType type, const EventHandler& handler);

    void triggerEvent(EventType type, void* data);

private:
    EventManager() = default;

    // GLFW callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    // Indexed by EventType
    std::vector<std::vector<EventHandler>> m_listeners = std::vector<std::vector<EventHandler>>(std::to_underlying(EventType::Count));

    friend class Window;
};
