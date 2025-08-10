cmake_minimum_required(VERSION 3.15)
project(2048 C)

set(CMAKE_C_STANDARD 99)

# Try to find Raylib installed in the system
find_package(raylib 4.0 QUIET)

# If not found, download and build Raylib automatically
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0  # Latest stable
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} raylib)

# OS-specific libraries
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework Cocoa" "-framework IOKit")
elseif (UNIX AND NOT APPLE) # Linux
    target_link_libraries(${PROJECT_NAME} m pthread dl GL X11)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32 winmm)
endif()
