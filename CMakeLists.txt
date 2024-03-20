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
    src/GLObject.cpp
    src/Camera.cpp
    src/Entity.cpp
    src/Game.cpp
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

# Add an executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set library directories
# target_link_directories(${PROJECT_NAME}
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/lib
# )

# Add any additional libraries your executable may depend on
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glm
        assimp
        # Add more libraries here as needed
)

# If you have any custom CMake modules, you can include them here
# include(CMakeModuleFile.cmake)

# Optionally, you can add additional compiler flags or definitions
# target_compile_options(YourExecutableName
#     PRIVATE
#         -Wall
# )

# Optionally, set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
