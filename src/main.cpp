
#define GL_SILENCE_DEPRECATION

#include <iostream>

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280,
        720,
        "3D Car Showroom",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    std::cout << "OpenGL initialized successfully!\n";
    std::cout << "Renderer: "
              << glGetString(GL_RENDERER) << '\n';
    std::cout << "Version: "
              << glGetString(GL_VERSION) << '\n';

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}