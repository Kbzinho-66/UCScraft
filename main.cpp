#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "camera.hpp"
#include "Block.hpp"

#include <iostream>
#include <vector>

void processInput(GLFWwindow* window);

void cameraCallback(GLFWwindow* window, double xpos, double ypos);

void mouseCallback(GLFWwindow* window, int button, int action, int mods);

void drawSkybox(Shader shader, GLuint texture);

void drawGround(Shader shader, GLuint texture);

void drawCursor(Shader shader, GLuint texture);

void drawBlock(Block block, Shader shader, GLuint* textures);

// Configurações da tela
const unsigned int WIDTH = 1080;
const unsigned int HEIGHT = 720;

// Constantes de texturas
#define SKY 0
#define GROUND 1
#define WOOD 2
#define LEAVES 3
#define TREE 4
#define STONE 5

int currentTexture = 2;

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// Variáveis de tempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Vetor com todos os blocos posicionados pelo jogador
std::vector<Block> blocks;

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
    glfwSetCursorPosCallback(window, cameraCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
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
    auto* textures = new GLuint[6];
    std::string textureFiles[6] = {
            "Sky.jpg",
            "Ground.jpg",
            "Wood.jpg",
            "Leaves.jpg",
            "Tree.jpg",
            "Stone.jpg"
    };

    glGenTextures(6, textures);
    for (int i = 0; i < 6; ++i) {
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
        auto currentFrame = (float) glfwGetTime();
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

        // Desenhar o mundo
        drawGround(shader, textures[GROUND]);
        drawCursor(shader, textures[currentTexture]);

        // Desenhar os blocos do jogador
        for (auto &block: blocks) {
            drawBlock(block, shader, textures);
        }

        // Desenhar a Skybox por último pra otimizar um pouco, já que vai ter umas quantas coisas cobrindo
        drawSkybox(shader, textures[SKY]);

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

void placeBlock() {
    auto newPos = camera.getClickPosition();

    // Se nenhum dos blocos já posicionados tiver a mesma posição do novo bloco, colocar ele no vetor
    if (std::none_of(blocks.begin(), blocks.end(),
                     [&newPos](Block alreadyPlaced) { return alreadyPlaced.Position == newPos; })) {
        blocks.emplace_back(newPos, currentTexture);

        printf("Novo bloco colocado em (%f, %f, %f)\n", newPos.x, newPos.y, newPos.z);
    }

}

void breakBlock() {
    auto clickPos = camera.getClickPosition();

    // Encontrar algum bloco com as mesmas coordenadas do clickPos
    auto selectedBlock = std::find_if(blocks.begin(), blocks.end(),
                                      [&clickPos](Block placed) { return placed.Position == clickPos; });

    if (selectedBlock != blocks.end()) {
        blocks.erase(selectedBlock);
        printf("Bloco em (%f, %f, %f) quebrado\n", clickPos.x, clickPos.y, clickPos.z);
    }
}

void drawSkybox(Shader shader, GLuint texture) {
    // Como o céu vai ficar "atrás" de literalmente qualquer coisa, não precisa guardar a profundidade dele
    glDepthMask(GL_FALSE);

    // Carregar a textura do céu
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Inicializar o modelo do Skybox
    auto model = glm::mat4(1.0f);
    // Deixar enorme e girando quase imperceptivelmente
    model = glm::scale(model, glm::vec3(100.0f));
    model = glm::rotate(model, (float) ( glfwGetTime() / 500.0 ), glm::vec3(0.0f, -0.5f, 0.0f));
    // Passar pro shader
    shader.setMat4("model", model);
    // Renderizar
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
}

void drawGround(Shader shader, GLuint texture) {
    // Carregar a textura
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Inicializar o modelo
    auto model = glm::mat4(1.0f);
    // Deslocar o chão pra alinhar com a face inferior dos blocos em y = 0
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(100.0f, 0.01f, 100.0f));
    // Passar pro shader
    shader.setMat4("model", model);
    // Renderizar
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawCursor(Shader shader, GLuint texture) {
    // Carregar a textura
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Inicializar o modelo
    auto model = glm::mat4(1.0f);
    // Colocar um indicador de onde vai ir o bloco novo
    model = glm::translate(model, camera.getClickPosition());
    model = glm::scale(model, glm::vec3(0.05));
    // Passar pro shader
    shader.setMat4("model", model);
    // Renderizar
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawBlock(Block block, Shader shader, GLuint* textures) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[block.Texture]);

    // Criar bloco e mover para a posição certa
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, block.Position);

    // Renderizar o bloco
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Função para tratamento das teclas relacionadas à movimentação
void processInput(GLFWwindow* window) {
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

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = 5.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.MovementSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        currentTexture = WOOD;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        currentTexture = LEAVES;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        currentTexture = TREE;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        currentTexture = STONE;
}

// Função pra tratar movimentos do mouse e mover a câmera de acordo
void cameraCallback(GLFWwindow* window, double xpos, double ypos) {
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

//    printf("Posição = (%f, %f, %f)\n", camera.Position.x, camera.Position.y, camera.Position.z);
//    printf("Front = (%f, %f, %f)\n", camera.Front.x, camera.Front.y, camera.Front.z);

}

// Função pra tratar os botões do mouse
void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        placeBlock();
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        breakBlock();
}