#include "Game.hpp"

#include "Camera.hpp"
#include "Entity.hpp"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Shader.hpp"
#include "Window.hpp"

#include <stb_image.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>

Game::Game()
    : window_(SCR_WIDTH, SCR_HEIGHT, "Aim Trainer GL", FULLSCREEN)
    , camera_({0.0, 1.5, 1.0}, {0.0, 1.0, 0.0}, -90.0, 0.0)
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

    stbi_set_flip_vertically_on_load(true);
    this->mouseButtons_.fill({GLFW_RELEASE, GLFW_RELEASE});

    this->resourceManager_.addShader("sprite",
                                     "../resources/shaders/sprite.vert",
                                     "../resources/shaders/sprite.frag");
    this->resourceManager_.addShader("entity",
                                     "../resources/shaders/model.vert",
                                     "../resources/shaders/color.frag");

    this->resourceManager_.addModel("cube",
                                    "../resources/objects/cube/cube.obj");

    this->spriteRenderer_ = std::make_unique<SpriteRenderer>(
        this->resourceManager_.getShader("sprite"));

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            Entity entity(this->resourceManager_.getModel("cube"),
                          glm::vec3(2 * i, 2 * j + 5.0, -15.0),
                          glm::vec4(0.2, 0.0, 1.0, 1.0));
            entity.addCollisionBox();
            entity.destroyable = true;
            entity.health = 3;
            this->entities_.push_back(std::move(entity));
        }
    }

    // crosshair
    // this->sprites_.push_back(
    //     std::make_unique<Sprite>(SQUARE, std::nullopt, glm::vec3(0.0, 0.0, 1.0),
    //                              glm::vec4(0.0, 1.0, 0.0, 1.0)));
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
    this->updateShotEntities();

    // move targets so they oscilate around their original position
    // on the x axis
    // for (auto& entity : this->entities_)
    // {
    //     float offset = 5 * sin(glfwGetTime());  // [-5, 5]
    //     entity->moveRelative(glm::vec3(offset, 0.0, 0.0));
    // }
}

void Game::updateShotEntities()
{
    if (this->mouseButtons_[0].current != GLFW_PRESS ||
        this->mouseButtons_[0].prev != GLFW_RELEASE)
    {
        return;
    }

    auto closestEntityIt = this->entities_.end();
    float closestDist = FLT_MAX;

    for (auto it = this->entities_.begin(); it != this->entities_.end(); it++)
    {
        const auto& entity = *it;
        if (!entity.getCollisionBox().has_value())
        {
            continue;
        }

        auto intersection = entity.getCollisionBox()->isIntersectedByLine(
            this->camera_.getPosition(), this->camera_.getFront());

        if (intersection.has_value())
        {
            float dist =
                glm::distance(this->camera_.getPosition(), *intersection);
            if (dist < closestDist)
            {
                closestDist = dist;
                closestEntityIt = it;
            }
        }
    }

    if (closestEntityIt != this->entities_.end() &&
        closestEntityIt->destroyable)
    {
        closestEntityIt->health--;
        if (closestEntityIt->health <= 0)
        {
            this->entities_.erase(closestEntityIt);
        }
        this->shotsHit_++;
    }

    this->totalShots_++;
}

void Game::render()
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Shader& entityShader = this->resourceManager_.getShader("entity");
    entityShader.use();

    glm::mat4 view = this->camera_.getViewMatrix();
    entityShader.setMat4("view", view);

    for (auto& entity : this->entities_)
    {
        auto model = glm::identity<glm::mat4>();
        model = glm::translate(model, entity.getCurrentPos());
        entityShader.setMat4("model", model);

        glm::mat4 projection = glm::perspective(
            glm::radians(this->camera_.getZoom()),
            (float)this->window_.getWidth() / this->window_.getHeight(), 0.1F,
            100.0F);
        entityShader.setMat4("projection", projection);

        entityShader.setVec4("color", entity.getColor());

        entity.render(entityShader);
    }

    const Shader& spriteShader = this->resourceManager_.getShader("sprite");
    spriteShader.use();

    glm::mat4 projection = glm::ortho(
        -this->window_.getWidth() / 2.0f, this->window_.getWidth() / 2.0f,
        -this->window_.getHeight() / 2.0f, this->window_.getHeight() / 2.0f,
        -1.0f, 1.0f);
    spriteShader.setMat4("projection", projection);

    this->spriteRenderer_->render(
        glm::vec2(-CROSSHAIR_SIZE_PX / 2, CROSSHAIR_SIZE_PX / 2), 0.0f,
        glm::vec2(CROSSHAIR_SIZE_PX, CROSSHAIR_SIZE_PX),
        glm::vec3(0.0f, 1.0f, 0.0f));

    // for (auto& sprite : this->sprites_)
    // {
    //     sprite->getShader().use();

    //     glm::mat4 model =
    //         glm::scale(glm::identity<glm::mat4>(), {0.015, 0.015, 1.0});
    //     sprite->getShader().setMat4("model", model);

    //     float aspectRatio =
    //         (float)this->window_.getWidth() / this->window_.getHeight();
    //     glm::mat4 projection =
    //         glm::ortho(-aspectRatio, aspectRatio, -1.0F, 1.0F);
    //     sprite->getShader().setMat4("projection", projection);

    //     auto view = glm::identity<glm::mat4>();
    //     sprite->getShader().setMat4("view", view);

    //     sprite->getShader().setVec4("color", sprite->getColor());

    //     sprite->render();
    // }
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
