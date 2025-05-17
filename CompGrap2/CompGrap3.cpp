#define GLEW_DLL
#define GLFW_DLL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ShaderLoader.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"
#include <iostream>

// Параметры камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Углы Эйлера
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;
float sensitivity = 0.1f;
float cameraSpeed = 5.0f;


float deltaTime = 0.0f;
float lastFrame = 0.0f;



float OX1 = 0.0f;
float OX2 = 0.0f;
float OX3 = 0.0f;

float OX_Z = 0.0f; 
float OX_X = 0.0f; 
float OX_Y = 0.0f;


float OX1_X = 0.0f, OX1_Y = 0.0f, OX1_Z = 0.0f;
glm::mat4 translateOX1 = glm::mat4(1.0f);


float OX2_Y = 0.0f; 
glm::mat4 translateOX2 = glm::mat4(1.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

   
    float animationSpeed = 3.0f * deltaTime;

    // Движение Стола (вдоль и поперек)
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        OX1_Z -= animationSpeed;
        OX1_Z = glm::clamp(OX1_Z, -0.13f, 0.115f);
        translateOX1 = glm::translate(glm::mat4(1.0f), glm::vec3(OX1_X, 0, OX1_Z));
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        OX1_Z += animationSpeed;
        OX1_Z = glm::clamp(OX1_Z, -0.13f, 0.115f);
        translateOX1 = glm::translate(glm::mat4(1.0f), glm::vec3(OX1_X, 0, OX1_Z));
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        OX1_X -= animationSpeed;
        OX1_X = glm::clamp(OX1_X, -0.25f, 0.25f);
        translateOX1 = glm::translate(glm::mat4(1.0f), glm::vec3(OX1_X, 0, OX1_Z));
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        OX1_X += animationSpeed;
        OX1_X = glm::clamp(OX1_X, -0.25f,0.25f);
        translateOX1 = glm::translate(glm::mat4(1.0f), glm::vec3(OX1_X, 0, OX1_Z));
    }

    // Движение Головы (вверх и вниз)
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        OX2_Y += animationSpeed;
        OX2_Y = glm::clamp(OX2_Y, -0.4875f, 0.0f);
        translateOX2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, OX2_Y, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        OX2_Y -= animationSpeed;
        OX2_Y = glm::clamp(OX2_Y, -0.4875f, 0.0f);
        translateOX2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, OX2_Y, 0));
    }

}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: GLFW initialization failed\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Model Viewer", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: Window creation failed\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);

    ShaderLoader shader("vertex_shader.glsl", "fragment_shader.glsl");
    Model ourModel("model.obj");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // Матрицы проекции и вида
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Базовые преобразования модели
        glm::mat4 baseModel = glm::mat4(1.0f);
        baseModel = glm::translate(baseModel, glm::vec3(0.0f, -1.75f, 0.0f));
        baseModel = glm::scale(baseModel, glm::vec3(0.2f, 0.2f, 0.2f));

        // Нормальная матрица
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(baseModel)));
        shader.SetUniformMatrix3fv("normalMatrix", glm::value_ptr(normalMatrix));

        // Передача матриц в шейдер
        shader.SetUniformMatrix4fv("projection", projection);
        shader.SetUniformMatrix4fv("view", view);

        // Настройка материала и света
        shader.SetUniform3f("viewPos", cameraPos);

        // Материал
        shader.SetUniform3f("material.ambient", 1.0f, 0.5f, 0.31f);
        shader.SetUniform3f("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.SetUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
        shader.SetUniform1f("material.shininess", 32.0f);

        // Источник света
        shader.SetUniform3f("light_1.Position", -3.2f, 4.0f, 4.0f);
        shader.SetUniform3f("light_1.ambient", 0.2f, 0.2f, 0.2f);
        shader.SetUniform3f("light_1.diffuse", 0.8f, 0.8f, 0.8f);
        shader.SetUniform3f("light_1.specular", 1.0f, 1.0f, 1.0f);

        // Отрисовка
        ourModel.Draw(shader,
            translateOX1, 
            translateOX2, 
            glm::mat4(1.0f), 
            baseModel
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}