#pragma once

#include "clay.h"

#include <cstdint>
#include <vector>

class ClayWrapper
{
public:
    ClayWrapper(float screenWidth, float screenHeight);

    Clay_RenderCommandArray buildRedSquare();
private:
    std::vector<uint8_t> m_clayBuffer;
};
