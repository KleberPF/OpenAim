#include "Game.hpp"

#include "Camera.hpp"
#include "Entity.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "objects.hpp"
#include "Sprite.hpp"
#include "Window.hpp"

#include <iostream>
#include <memory>
#include <optional>

Game::Game()
    : window_(SCR_WIDTH, SCR_HEIGHT, "Aim Trainer GL", FULLSCREEN)
    , camera_({0.0, 1.5, 1.0}, {0.0, 1.0, 0.0}, -90.0, 0.0)
    , mouseButtons_()
    , lastX_((float)this->window_.getWidth() / 2)
    , lastY_((float)this->window_.getHeight() / 2)
{
    // opengl stuff
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, nullptr);

    this->mouseButtons_.fill({GLFW_RELEASE, GLFW_RELEASE});

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            this->entities_.push_back(std::make_unique<Target>(
                CUBE, std::nullopt, glm::vec3(2 * i, 2 * j + 5.0, -15.0),
                glm::vec4(0.2, 0.0, 1.0, 1.0)));
        }
    }

    // crosshair
    this->sprites_.push_back(std::make_unique<Sprite>(
        SQUARE, std::nullopt, glm::vec3(0.0, 0.0, 1.0)));
}

void Game::mainLoop()
{
    while (!this->window_.shouldClose())
    {
        this->processTiming();
        this->processInput();
        this->render();
        this->updateGlfw();
    }
}

void Game::processTiming()
{
    auto currentFrame = (float)glfwGetTime();
    this->deltaTime_ = currentFrame - this->lastFrame_;
    this->lastFrame_ = currentFrame;
}

void Game::processInput()
{
    // mouse input
    double xposIn = 0;
    double yposIn = 0;
    glfwGetCursorPos(this->window_.getPtr(), &xposIn, &yposIn);

    auto xpos = (float)xposIn;
    auto ypos = (float)yposIn;

    if (this->firstMouse_)
    {
        this->lastX_ = xpos;
        this->lastY_ = ypos;
        this->firstMouse_ = false;
    }

    float xoffset = xpos - this->lastX_;
    float yoffset = this->lastY_ -
                    ypos;  // reversed since y-coordinates go from bottom to top

    this->lastX_ = xpos;
    this->lastY_ = ypos;

    this->mouseButtons_[0].current =
        glfwGetMouseButton(this->window_.getPtr(), GLFW_MOUSE_BUTTON_LEFT);
    this->mouseButtons_[1].current =
        glfwGetMouseButton(this->window_.getPtr(), GLFW_MOUSE_BUTTON_RIGHT);

    // notify entities when hit
    this->updateEntities();

    this->mouseButtons_[0].prev = this->mouseButtons_[0].current;
    this->mouseButtons_[1].prev = this->mouseButtons_[1].current;

    // camera mouse processing
    this->camera_.processMouseMovement(xoffset, yoffset);

    // generic processing
    if (glfwGetKey(this->window_.getPtr(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(this->window_.getPtr(), true);
    }

    // camera keyboard processing
    // uncomment this to allow flying around
    if (glfwGetKey(this->window_.getPtr(), GLFW_KEY_W) == GLFW_PRESS)
    {
        this->camera_.processKeyboard(CameraMovement::FORWARD,
                                      this->deltaTime_);
    }
    if (glfwGetKey(this->window_.getPtr(), GLFW_KEY_S) == GLFW_PRESS)
    {
        this->camera_.processKeyboard(CameraMovement::BACKWARD,
                                      this->deltaTime_);
    }
    if (glfwGetKey(this->window_.getPtr(), GLFW_KEY_A) == GLFW_PRESS)
    {
        this->camera_.processKeyboard(CameraMovement::LEFT, this->deltaTime_);
    }
    if (glfwGetKey(this->window_.getPtr(), GLFW_KEY_D) == GLFW_PRESS)
    {
        this->camera_.processKeyboard(CameraMovement::RIGHT, this->deltaTime_);
    }
}

void Game::updateEntities()
{
    if (this->mouseButtons_[0].current == GLFW_PRESS &&
        this->mouseButtons_[0].prev == GLFW_RELEASE)
    {
        BaseEntity* closestEntity = nullptr;
        float closestDist = FLT_MAX;

        for (auto& entity : this->entities_)
        {
            bool intersects = false;
            auto intersection = entity->isIntersectedByLine(
                this->camera_.getPosition(), this->camera_.getFront());

            if (intersection.has_value())
            {
                float dist =
                    glm::distance(this->camera_.getPosition(), *intersection);
                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestEntity = entity.get();
                }
            }
        }

        if (closestEntity != nullptr)
        {
            closestEntity->onHit();
            this->shotsHit_++;
        }

        this->totalShots_++;
    }
}

void Game::render()
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& entity : this->entities_)
    {
        entity->getShader().use();

        glm::mat4 model =
            glm::translate(glm::identity<glm::mat4>(), entity->getPos());
        entity->getShader().setMat4("model", model);

        glm::mat4 projection = glm::perspective(
            glm::radians(this->camera_.getZoom()),
            (float)this->window_.getWidth() / this->window_.getHeight(), 0.1F,
            100.0F);
        entity->getShader().setMat4("projection", projection);

        glm::mat4 view = this->camera_.getViewMatrix();
        entity->getShader().setMat4("view", view);

        entity->getShader().setVec4("color", entity->getColor());

        entity->render();
    }

    for (auto& sprite : this->sprites_)
    {
        sprite->getShader().use();

        glm::mat4 model =
            glm::scale(glm::identity<glm::mat4>(), {0.015, 0.015, 1.0});
        sprite->getShader().setMat4("model", model);

        float aspectRatio =
            (float)this->window_.getWidth() / this->window_.getHeight();
        glm::mat4 projection =
            glm::ortho(-aspectRatio, aspectRatio, -1.0F, 1.0F);
        sprite->getShader().setMat4("projection", projection);

        auto view = glm::identity<glm::mat4>();
        sprite->getShader().setMat4("view", view);

        sprite->getShader().setVec4("color", {0.0, 1.0, 0.0, 1.0});

        sprite->render();
    }
}

void Game::updateGlfw()
{
    glfwSwapBuffers(this->window_.getPtr());
    glfwPollEvents();
}

void messageCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/,
                     GLenum severity, GLsizei /*unused*/, const GLchar* message,
                     const void* /*unused*/)
{
    if (severity == GL_DEBUG_SEVERITY_MEDIUM ||
        severity == GL_DEBUG_SEVERITY_HIGH)
    {
        std::cerr << "GL CALLBACK: "
                  << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
                  << " type = 0x" << std::hex << type << ", severity = 0x"
                  << severity << ", message = " << message << std::endl;
    }
}
