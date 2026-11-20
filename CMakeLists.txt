
cmake_minimum_required(VERSION 3.20)

project(CarShowroom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(CarShowroom
    src/main.cpp
    src/shader.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/TextureLoader.cpp
)

target_link_libraries(CarShowroom PRIVATE
    glfw
    glm::glm
    OpenGL::GL
    assimp::assimp
)

target_include_directories(CarShowroom PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)