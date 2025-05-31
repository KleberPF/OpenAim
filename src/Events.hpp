#pragma once

#include <string>

struct ResizeEvent {
    int width;
    int height;
};

struct KeyPressEvent {
    int key;
    bool pressed;
};

struct MouseButtonEvent {
    int button;
    bool pressed;
};

struct CursorPosEvent {
    double xpos;
    double ypos;
};

struct NewScenarioEvent {
    std::string scenario;
    bool challenge;
};
