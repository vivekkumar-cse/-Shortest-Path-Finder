cmake_minimum_required(VERSION 3.16)
project(RoninPathfinder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8 /O2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# --- Dependencies via FetchContent (no manual install required) ---
include(FetchContent)

set(SFML_VERSION 2.6.1)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG ${SFML_VERSION}
    GIT_SHALLOW TRUE
)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ImGui)

FetchContent_Declare(
    ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v2.6
    GIT_SHALLOW TRUE
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
set(IMGUI_SFML_IMGUI_DEMO ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ImGui-SFML)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

# --- Sources ---
file(GLOB_RECURSE RONIN_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(RoninPathfinder ${RONIN_SOURCES})

target_include_directories(RoninPathfinder PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(RoninPathfinder PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
    nlohmann_json::nlohmann_json
)

if(WIN32)
    add_custom_command(TARGET RoninPathfinder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:RoninPathfinder>
    )
endif()

# Copy assets next to binary
add_custom_command(TARGET RoninPathfinder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:RoninPathfinder>/assets
)
