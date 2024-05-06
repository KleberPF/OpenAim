# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name
project(OpenAim)

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
    src/NuklearWrapper.cpp
    src/EntityManager.cpp
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

# Add nlohmann_json submodule
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/nlohmann_json)

# Add an executable target
if (MSVC)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# Set include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
        OpenAL::OpenAL
        nlohmann_json::nlohmann_json
        # Add more libraries here as needed
)

# If you have any custom CMake modules, you can include them here
# include(CMakeModuleFile.cmake)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            $<$<CONFIG:Debug>:OE_DEBUG>
            $<$<CONFIG:RelWithDebInfo>:OE_DEBUG>
            $<$<CONFIG:Release>:OE_RELEASE>
            $<$<CONFIG:MinSizeRel>:OE_RELEASE>
)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Optionally, set C++ standard
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

if (MSVC)
    if (OE_DEBUG)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/openal-soft/OpenAL32d.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/openal-soft/OpenAL32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
