#include "Game.hpp"

#include <glad/glad.h>

#include <cstdlib>
#include <ctime>

int main()
{
    srand(time(nullptr));
    Game game;
    game.mainLoop();
}