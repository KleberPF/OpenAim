#include "Game.hpp"

#include "Camera.hpp"
#include "Entity.hpp"
#include "GLFW/glfw3.h"
#include "InputManager.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <stb_image.h>

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>

Game::Game()
    : m_window(SCR_WIDTH, SCR_HEIGHT, "Aim Trainer GL", FULLSCREEN)
    , m_camera({ 0.0f, 0.0f, 0.0f }, { 0.0, 1.0, 0.0 }, -90.0, 0.0)
    , m_inputManager(m_window)
    , m_lastX((float)m_window.getWidth() / 2)
    , m_lastY((float)m_window.getHeight() / 2)
{
    // opengl initialization
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, nullptr);

    InputManager::setupInputCallbacks(m_window.getPtr());

    // ImGui initialization
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_window.getPtr(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // load shaders and models
    m_resourceManager.addShader("sprite", "../resources/shaders/sprite.vert",
        "../resources/shaders/sprite.frag");
    m_resourceManager.addShader("textured", "../resources/shaders/model.vert",
        "../resources/shaders/model.frag");
    m_resourceManager.addShader("targets", "../resources/shaders/model.vert",
        "../resources/shaders/color.frag");
    m_resourceManager.addShader("skybox", "../resources/shaders/skybox.vert",
        "../resources/shaders/skybox.frag");

    m_resourceManager.addCubemap("skybox",
        { "../resources/textures/skybox/right.bmp",
            "../resources/textures/skybox/left.bmp",
            "../resources/textures/skybox/top.bmp",
            "../resources/textures/skybox/bottom.bmp",
            "../resources/textures/skybox/front.bmp",
            "../resources/textures/skybox/back.bmp" });

    m_resourceManager.addTexture("checkerboard",
        "../resources/textures/checkerboard.png", Texture::Type::Diffuse);
    m_resourceManager.addTexture(
        "bricks", "../resources/textures/bricks.jpg", Texture::Type::Diffuse);

    m_resourceManager.addMaterial("checkerboard");
    m_resourceManager.getMaterial("checkerboard")
        .addTexture(m_resourceManager.getTexture("checkerboard"));

    m_resourceManager.addMaterial("bricks");
    m_resourceManager.getMaterial("bricks").addTexture(
        m_resourceManager.getTexture("bricks"));

    m_resourceManager.addModel("cube", "../resources/objects/cube/cube.obj",
        m_resourceManager.getMaterial("checkerboard"),
        m_resourceManager.getShader("targets"));
    m_resourceManager.addModel("ball", "../resources/objects/ball/ball.obj",
        m_resourceManager.getMaterial("checkerboard"),
        m_resourceManager.getShader("targets"));
    m_resourceManager.addModel("plane", "../resources/objects/plane2/plane.obj",
        m_resourceManager.getMaterial("bricks"),
        m_resourceManager.getShader("textured"));

    m_renderer = std::make_unique<Renderer>(m_window, m_camera);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            Entity entity(m_resourceManager.getModel("cube"),
                glm::vec3(2.0f * i, 2.0f * j, -18.0f));
            entity.addCollisionObject(CollisionObjectType::AABB);
            entity.setSize(glm::vec3(0.5f));
            entity.setColor(glm::vec3(0.125f, 0.55f, 0.9f));
            entity.setRotation(30.0f, 30.0f, 0);
            entity.setDestroyable(true);
            m_entities.push_back(std::move(entity));
        }
    }

    Entity plane(
        m_resourceManager.getModel("plane"), glm::vec3(0.0f, 0.0f, -20.0f));
    plane.addCollisionObject(CollisionObjectType::AABB);
    plane.setRotation(90, 0, 0);
    plane.setSize(glm::vec3(10.0f, 1.0f, 10.0f));
    plane.setColor(glm::vec3(1.0f, 0.0, 0.0f));
    plane.setDestroyable(false);
    m_entities.push_back(std::move(plane));

    Entity plane2(
        m_resourceManager.getModel("plane"), glm::vec3(-10.0f, 0.0f, -10.0f));
    plane2.addCollisionObject(CollisionObjectType::AABB);
    plane2.setRotation(90, 90, 0);
    plane2.setSize(glm::vec3(10.0f, 1.0f, 10.0f));
    plane2.setColor(glm::vec3(1.0f, 0.0, 0.0f));
    plane2.setDestroyable(false);
    m_entities.push_back(std::move(plane2));

    Entity plane3(
        m_resourceManager.getModel("plane"), glm::vec3(10.0f, 0.0f, -10.0f));
    plane3.addCollisionObject(CollisionObjectType::AABB);
    plane3.setRotation(90, -90, 0);
    plane3.setSize(glm::vec3(10.0f, 1.0f, 10.0f));
    plane3.setColor(glm::vec3(1.0f, 0.0, 0.0f));
    plane3.setDestroyable(false);
    m_entities.push_back(std::move(plane3));
}

void Game::mainLoop()
{
    while (!m_window.shouldClose()) {
        processTiming();
        processInput();
        render();
        updateGlfw();
    }
}

void Game::processTiming()
{
    auto currentFrame = (float)glfwGetTime();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
}

void Game::processInput()
{
    // mouse input
    auto [xpos, ypos] = m_inputManager.getCursorPos();

    if (m_firstMouse) {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset
        = m_lastY - ypos; // reversed since y-coordinates go from bottom to top

    m_lastX = xpos;
    m_lastY = ypos;

    if (m_inputManager.isKeyToggled(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_window.getPtr(), true);
        return;
    }

    if (m_inputManager.isKeyToggled(GLFW_KEY_TAB)) {
        m_paused = !m_paused;
    }

    if (!m_paused) {
        updateEntities();
        m_camera.processMouseMovement(xoffset, yoffset);
        glfwSetInputMode(m_window.getPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // camera keyboard processing
        // uncomment this to allow flying around
        if (m_inputManager.isKeyPressed(GLFW_KEY_W)) {
            m_camera.processKeyboard(CameraMovement::FORWARD, m_deltaTime);
        }
        if (m_inputManager.isKeyPressed(GLFW_KEY_S)) {
            m_camera.processKeyboard(CameraMovement::BACKWARD, m_deltaTime);
        }
        if (m_inputManager.isKeyPressed(GLFW_KEY_A)) {
            m_camera.processKeyboard(CameraMovement::LEFT, m_deltaTime);
        }
        if (m_inputManager.isKeyPressed(GLFW_KEY_D)) {
            m_camera.processKeyboard(CameraMovement::RIGHT, m_deltaTime);
        }
    } else {
        glfwSetInputMode(m_window.getPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    m_inputManager.consolidateKeyStates();
}

void Game::updateEntities()
{
    updateShotEntities();

    // move targets so they oscilate around their original position
    // on the x axis
    // for (auto& entity : m_entities)
    // {
    //     float offset = 5 * sin(glfwGetTime());  // [-5, 5]
    //     entity->moveRelative(glm::vec3(offset, 0.0, 0.0));
    // }
}

void Game::updateShotEntities()
{
    if (!m_inputManager.isMouseButtonToggled(GLFW_MOUSE_BUTTON_LEFT)) {
        return;
    }

    auto closestEntityIt = m_entities.end();
    float closestDist = FLT_MAX;

    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        const auto& entity = *it;

        auto intersection = entity.getCollisionObject()->isIntersectedByLine(
            m_camera.getPosition(), m_camera.getFront());

        if (intersection.has_value()) {
            float dist = glm::distance(m_camera.getPosition(), *intersection);
            if (dist < closestDist) {
                closestDist = dist;
                closestEntityIt = it;
            }
        }
    }

    if (closestEntityIt != m_entities.end()
        && closestEntityIt->isDestroyable()) {
        closestEntityIt->setHealth(closestEntityIt->getHealth() - 1);
        if (closestEntityIt->getHealth() <= 0) {
            m_entities.erase(closestEntityIt);
        }
        m_shotsHit++;
    }

    m_totalShots++;
}

void Game::render()
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& entity : m_entities) {
        m_renderer->renderEntity(entity);
    }

    m_renderer->renderSprite(m_resourceManager.getShader("sprite"),
        glm::vec2(-CROSSHAIR_SIZE_PX / 2, CROSSHAIR_SIZE_PX / 2), 0.0f,
        glm::vec2(CROSSHAIR_SIZE_PX, CROSSHAIR_SIZE_PX),
        glm::vec3(0.0f, 1.0f, 0.0f));

    m_renderer->renderSkybox(m_resourceManager.getShader("skybox"),
        m_resourceManager.getCubemap("skybox"));

    // render ImGui stuff
    if (m_paused) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void Game::updateGlfw()
{
    glfwSwapBuffers(m_window.getPtr());
    glfwPollEvents();
}

void messageCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/,
    GLenum severity, GLsizei /*unused*/, const GLchar* message,
    const void* /*unused*/)
{
    if (severity == GL_DEBUG_SEVERITY_MEDIUM
        || severity == GL_DEBUG_SEVERITY_HIGH) {
        std::cerr << "GL CALLBACK: "
                  << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
                  << " type = 0x" << std::hex << type << ", severity = 0x"
                  << severity << ", message = " << message << std::endl;
    }
}
