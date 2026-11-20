
cmake_minimum_required(VERSION 3.20)

project(CarShowroom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(CarShowroom
    src/main.cpp
)

target_link_libraries(CarShowroom PRIVATE
    glfw
    glm::glm
    OpenGL::GL
)

target_include_directories(CarShowroom PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)