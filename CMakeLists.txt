cmake_minimum_required(VERSION 3.3)

project(plotter)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})
add_definitions(${SDL2_DEFINITIONS})

find_package(OpenGL)

set(COMMON_SRC
    "src/main.cpp"
    "src/motor.cpp"
    "src/motorcontroller.cpp"
    )

include_directories(
    "src/"
    ${SDL_INCLUDE_DIR}
    )


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

add_executable(${CMAKE_PROJECT_NAME} ${COMMON_SRC})
message("${SDL_LIBRARY}")
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    )
