#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <iostream>
#include <vector>
#include "camera.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

// Configurações da tela
const unsigned int WIDTH = 1080;
const unsigned int HEIGHT = 720;

// Constantes de texturas
#define SKY 0
#define GROUND 1
#define CONCRETE 2
#define ICE 3
#define WOOD 4

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// Variáveis de tempo
double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main() {
    // Inicializar e configurar o GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criar e configurar a janela
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "UCScraft", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Erro ao criar a janela do GLFW." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    // Não mostrar o cursor na tela
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializar o GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "Ocorreu um erro iniciando o GLEW!" << std::endl;
    } else {
        std::cout << "GLEW OK!" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    // Configurações globais do OpenGL
    glEnable(GL_DEPTH_TEST);

    // Preparar o shader
    Shader shader("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");

    // Conjuntos de vértices
    GLfloat blockVertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    };

    // Criar o modelo de um bloco
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof( blockVertices ), blockVertices, GL_STATIC_DRAW);

    // Definir coordenadas dos vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) nullptr);
    glEnableVertexAttribArray(0);
    // Definir coordenadas da textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) ( 3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Carregar as texturas
    auto* textures = new GLuint[5];
    std::string textureFiles[5] = {
            "Sky.jpg",
            "Ground.jpg",
            "Concrete.jpg",
            "Ice.jpg",
            "Wood.jpg"
    };

    glGenTextures(5, textures);
    for (int i = 0; i < 5; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        // Configurações da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Carregar a imagem e criar uma textura a partir dela
        int texWidth, texHeight, nrChannels;
        std::string texName = "../res/textures/" + textureFiles[i];
        // Inverter a imagem quando ler
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(texName.c_str(), &texWidth, &texHeight, &nrChannels, 0);
        if (data) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Erro ao carregar a textura " << texName << std::endl;
        }
        stbi_image_free(data);
    }

    // Loop principal de renderização
    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Processar as teclas de movimentação
        processInput(window);

        // Limpar a tela
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Ativar o shader de fato
        shader.use();

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f, 200.0f);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        /*************************** Desenhar o Skybox ***************************/
        // Carregar a textura do céu
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[SKY]);
        // Inicializar o modelo do Skybox
        glm::mat4 model = glm::mat4(1.0f);
        // Deixar enorme e girando quase imperceptivelmente
        model = glm::scale(model, glm::vec3(100.0f));
        model = glm::rotate(model, (float) ( glfwGetTime() / 500.0 ), glm::vec3(0.0f, -0.5f, 0.0f));
        // Passar pro shader
        shader.setMat4("model", model);
        // Renderizar
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /*************************** Desenhar o chão ***************************/
        // Carregar a textura
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[GROUND]);
        // Inicializar o modelo
        model = glm::mat4(1.0f);
        // Deslocar o chão pra alinhar com a face inferior dos blocos em y = 0
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        // O chão pode ter a mesma área do Skybox
        model = glm::scale(model, glm::vec3(100.0f, 0.01f, 100.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // Desenhar os blocos do jogador
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[ICE]);

//        for (unsigned int i = 0; i < 1; i++) {
        model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(angle) + glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
//        }

        // Mostrar as mudanças na tela
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar todos os recursos usados
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete[] textures;

    glfwTerminate();
    return 0;
}

// Função para tratamento das teclas relacionadas à movimentação
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

// Função pra tratar mudanças no tamanho da tela
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// Função pra tratar movimentos do mouse e mover a câmera de acordo
void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Inverter o y porque ele "cresce pra baixo"

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
