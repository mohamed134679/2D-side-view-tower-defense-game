cmake_minimum_required(VERSION 3.16)
project(Template LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(game main.cpp src/glad.c
    Player_type.h
    Player.h
    Enemy.h
    Enemy_type.h
    Projectiles.h
    Projectile_type.h
    Collectibles.h
    miniaudio.h)
target_include_directories(game PRIVATE . include)
target_link_libraries(game PRIVATE glfw OpenGL::GL)

# copy the sound files next to the game so it finds them when run from build/
file(COPY sounds DESTINATION ${CMAKE_BINARY_DIR})
