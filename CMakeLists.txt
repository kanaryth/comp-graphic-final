
cmake_minimum_required(VERSION 3.16)

project(CarShowroom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

add_executable(CarShowroom
    src/main.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/shader.cpp
)

target_include_directories(CarShowroom PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(CarShowroom PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    assimp::assimp
)
