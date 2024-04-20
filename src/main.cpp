#include "Game.hpp"

#include <cstdlib>
#include <ctime>
#include <glad/glad.h>

int main()
{
    srand(time(nullptr));
    Game game;
    game.mainLoop();
}