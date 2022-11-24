#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "include/shader_m.h"
#include "include/camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void processInput(GLFWwindow *window);

// Configurações de Tela
const unsigned int WIDTH = 1080;
const unsigned int HEIGHT = 720;

// Variáveis pra câmera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// Variáveis de tempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Variáveis pro Sol
double x, y, z;
const double radius = 10.0f;
const double speed = 0.01;

int main() {
    // Inicializar e configurar
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criar a janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "UCScraft", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Erro ao criar a janela GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Capturar o mouse com GLFW
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializar o GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "Ocorreu um erro iniciando GLEW!" << std::endl;
    } else {
        std::cout << "GLEW OK!" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    // Configurações globais do OpenGL
    glEnable(GL_DEPTH_TEST);

    // Preparar os shaders
    Shader lightingShader("../res/shaders/basic_lighting.vs", "../res/shaders/basic_lighting.fs");
    Shader lightCubeShader("../res/shaders/light_cube.vs", "../res/shaders/light_cube.fs");

    // Preparar os dados de vértices
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f , 0.0f , -1.0f,
        0.5f , -0.5f, -0.5f, 0.0f , 0.0f , -1.0f,
        0.5f , 0.5f , -0.5f, 0.0f , 0.0f , -1.0f,
        0.5f , 0.5f , -0.5f, 0.0f , 0.0f , -1.0f,
        -0.5f, 0.5f , -0.5f, 0.0f , 0.0f , -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f , 0.0f , -1.0f,

        -0.5f, -0.5f, 0.5f , 0.0f , 0.0f , 1.0f,
        0.5f , -0.5f, 0.5f , 0.0f , 0.0f , 1.0f,
        0.5f , 0.5f , 0.5f , 0.0f , 0.0f , 1.0f,
        0.5f , 0.5f , 0.5f , 0.0f , 0.0f , 1.0f,
        -0.5f, 0.5f , 0.5f , 0.0f , 0.0f , 1.0f,
        -0.5f, -0.5f, 0.5f , 0.0f , 0.0f , 1.0f,

        -0.5f, 0.5f , 0.5f , -1.0f, 0.0f , 0.0f,
        -0.5f, 0.5f , -0.5f, -1.0f, 0.0f , 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f , 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f , 0.0f,
        -0.5f, -0.5f, 0.5f , -1.0f, 0.0f , 0.0f,
        -0.5f, 0.5f , 0.5f , -1.0f, 0.0f , 0.0f,

        0.5f , 0.5f , 0.5f , 1.0f , 0.0f , 0.0f,
        0.5f , 0.5f , -0.5f, 1.0f , 0.0f , 0.0f,
        0.5f , -0.5f, -0.5f, 1.0f , 0.0f , 0.0f,
        0.5f , -0.5f, -0.5f, 1.0f , 0.0f , 0.0f,
        0.5f , -0.5f, 0.5f , 1.0f , 0.0f , 0.0f,
        0.5f , 0.5f , 0.5f , 1.0f , 0.0f , 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f , -1.0f, 0.0f,
        0.5f , -0.5f, -0.5f, 0.0f , -1.0f, 0.0f,
        0.5f , -0.5f, 0.5f , 0.0f , -1.0f, 0.0f,
        0.5f , -0.5f, 0.5f , 0.0f , -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f , 0.0f , -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f , -1.0f, 0.0f,

        -0.5f, 0.5f , -0.5f, 0.0f , 1.0f , 0.0f,
        0.5f , 0.5f , -0.5f, 0.0f , 1.0f , 0.0f,
        0.5f , 0.5f , 0.5f , 0.0f , 1.0f , 0.0f,
        0.5f , 0.5f , 0.5f , 0.0f , 1.0f , 0.0f,
        -0.5f, 0.5f , 0.5f , 0.0f , 1.0f , 0.0f,
        -0.5f, 0.5f , -0.5f, 0.0f , 1.0f , 0.0f
    };

    // Configurar o VBO e VAO de um Cubo
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // Definir a posição do cubo
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // Definir o vetor Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Configurar o VAO do sol, o VBO é o mesmo, porque é um sol quadrado :)
    unsigned int sunVAO;
    glGenVertexArrays(1, &sunVAO);
    glBindVertexArray(sunVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // Loop Principal
    while (!glfwWindowShouldClose(window)) {
        // Lógica calculada frame a frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        x = -sin(currentFrame * speed) * radius;
        y = cos(currentFrame * speed) * radius;
        z = 0.0;
        glm::vec3 sunPosition = glm::vec3(x, y, z);

        // Tratar algum input que tiver
        processInput(window);

        // Renderizar
        glClearColor(0.698f, 0.776f, 0.902f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Preparar o shader do Sol
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f,0.655f,0.0f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("sunPosition", sunPosition);

        // Cálculos de perspectiva
        glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                (float) WIDTH / (float) HEIGHT,
                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Transformação do mundo
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // Renderizar o cubo
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Renderizar o Sol
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, sunPosition);
        model = glm::scale(model, glm::vec3(0.3f));
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(sunVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Carregar o buffer novo e ver se não teve algum input
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar todos os recursos
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

