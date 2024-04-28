# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name
project(AimTrainerGL)

# List of source files
set(SOURCES
    src/main.cpp
    src/glad.c
    src/Window.cpp
    src/filereader.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Entity.cpp
    src/Game.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/stb_image.c
    src/stb_vorbis.c
    src/Renderer.cpp
    src/ResourceManager.cpp
    src/InputManager.cpp
    src/Material.cpp
    src/utils.cpp
    src/Sprite.cpp
    src/Scene.cpp
    src/Weapon.cpp
    src/RNG.cpp
    src/SoundPlayer.cpp
    src/Sound.cpp
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
    # Add more source files here as needed
)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add GLFW submodule
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)

# Add GLM submodule
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/glm)

# Add assimp submodule
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp)

# Add openal-soft submodule
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/openal-soft)

# Add an executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set library directories
target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Add any additional libraries your executable may depend on
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glm
        assimp
        openal
        # Add more libraries here as needed
)

# If you have any custom CMake modules, you can include them here
# include(CMakeModuleFile.cmake)

# Optionally, you can add additional compiler flags or definitions
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
)

# Optionally, set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
