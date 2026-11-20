
#define GL_SILENCE_DEPRECATION

#include <iostream>

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

float lastX = 640.0f;
float lastY = 360.0f;

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;

    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;

    if (fov < 1.0f)
        fov = 1.0f;

    if (fov > 45.0f)
        fov = 45.0f;
}

void processInput(GLFWwindow* window)
{
    float cameraSpeed = 3.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720,
        "3D Car Showroom",
        nullptr, nullptr
    );

    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    Shader shader(
        "shaders/vertex.glsl",
        "shaders/fragment.glsl"
    );

    if (shader.ID == 0)
    {
        std::cerr << "Shader creation failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    float vertices[] = {
        -0.5f,-0.5f,-0.5f, 1,0,0,
         0.5f,-0.5f,-0.5f, 0,1,0,
         0.5f, 0.5f,-0.5f, 0,0,1,
         0.5f, 0.5f,-0.5f, 0,0,1,
        -0.5f, 0.5f,-0.5f, 1,1,0,
        -0.5f,-0.5f,-0.5f, 1,0,0,

        -0.5f,-0.5f, 0.5f, 1,0,1,
         0.5f,-0.5f, 0.5f, 0,1,1,
         0.5f, 0.5f, 0.5f, 1,1,1,
         0.5f, 0.5f, 0.5f, 1,1,1,
        -0.5f, 0.5f, 0.5f, 1,0,0,
        -0.5f,-0.5f, 0.5f, 1,0,1,

        -0.5f, 0.5f, 0.5f, 1,0,0,
        -0.5f, 0.5f,-0.5f, 0,1,0,
        -0.5f,-0.5f,-0.5f, 0,0,1,
        -0.5f,-0.5f,-0.5f, 0,0,1,
        -0.5f,-0.5f, 0.5f, 1,1,0,
        -0.5f, 0.5f, 0.5f, 1,0,0,

         0.5f, 0.5f, 0.5f, 1,0,1,
         0.5f, 0.5f,-0.5f, 0,1,1,
         0.5f,-0.5f,-0.5f, 1,1,1,
         0.5f,-0.5f,-0.5f, 1,1,1,
         0.5f,-0.5f, 0.5f, 1,0,0,
         0.5f, 0.5f, 0.5f, 1,0,1,

        -0.5f,-0.5f,-0.5f, 0,1,0,
         0.5f,-0.5f,-0.5f, 1,0,0,
         0.5f,-0.5f, 0.5f, 0,0,1,
         0.5f,-0.5f, 0.5f, 0,0,1,
        -0.5f,-0.5f, 0.5f, 1,1,0,
        -0.5f,-0.5f,-0.5f, 0,1,0,

        -0.5f, 0.5f,-0.5f, 1,0,0,
         0.5f, 0.5f,-0.5f, 0,1,0,
         0.5f, 0.5f, 0.5f, 0,0,1,
         0.5f, 0.5f, 0.5f, 0,0,1,
        -0.5f, 0.5f, 0.5f, 1,1,0,
        -0.5f, 0.5f,-0.5f, 1,0,0
    };

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if (width == 0 || height == 0)
        {
            glfwPollEvents();
            continue;
        }

        glViewport(0, 0, width, height);

        glClearColor(0.06f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glm::mat4 model = glm::rotate(
            glm::mat4(1.0f),
            (float)glfwGetTime(),
            glm::vec3(0.5f, 1.0f, 0.0f)
        );

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(fov),
            (float)width / (float)height,
            0.1f,
            100.0f
        );

        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}