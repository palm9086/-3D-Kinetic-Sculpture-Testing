#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <ctime>

#define maxPadoru 100000

// Callbacks declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

glm::vec3 pointLightPositions[] = {
    glm::vec3(5.0f,0.0f,0.0f),
    glm::vec3(0.0f,5.0f,0.0f),
    glm::vec3(0.0f,0.0f,5.0f),
    glm::vec3(0.0f,0.0f,0.0f),
    glm::vec3(-5.0f,0.0f,0.0f),
    glm::vec3(0.0f,-5.0f,0.0f),
    glm::vec3(0.0f,0.0f,-5.0f)
};

glm::vec3 pointLightColors[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f)
};

// Padoru Control

int padoruCount = 0;
float spawnTime[maxPadoru] = { 0.0f };
glm::vec3 padoruPositions[maxPadoru];
float padoruRotation[maxPadoru] = { 0.0f };
float Speed[maxPadoru] = { 0.0f };
glm::vec3 padoruRotationdirection[maxPadoru];

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

int main()
{
    srand((unsigned)time(0));

    // GLFW Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Padoru Scene", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Shaders
    Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
    Shader lightCubeShader("6.light_cube.vs", "6.light_cube.fs");

    // Model
    Model padoruModel(FileSystem::getPath("resources/objects/padoru/padoru.obj"));

    // Light cube VAO
    float lightCubeVertices[] = {
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,-0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,-0.5f,  0.5f,  0.5f,-0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,-0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,-0.5f, -0.5f,  0.5f,-0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f, 0.5f,  0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,-0.5f, -0.5f,  0.5f,-0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,-0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f
    };

    unsigned int lightCubeVAO, lightVBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Timing
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Lighting
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        for (int i = 0; i < 4; ++i) {
            std::string num = std::to_string(i);
            lightingShader.setVec3("pointLights[" + num + "].position", pointLightPositions[i]);
            lightingShader.setVec3("pointLights[" + num + "].ambient", 0.05f * pointLightColors[i]);
            lightingShader.setVec3("pointLights[" + num + "].diffuse", 0.8f * pointLightColors[i]);
            lightingShader.setVec3("pointLights[" + num + "].specular", pointLightColors[i]);
            lightingShader.setFloat("pointLights[" + num + "].constant", 1.0f);
            lightingShader.setFloat("pointLights[" + num + "].linear", 0.09f);
            lightingShader.setFloat("pointLights[" + num + "].quadratic", 0.032f);
        }

        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // Matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Render Padorus
        for (int i = 0; i < padoruCount; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, padoruPositions[i]);

            float elapsed = currentFrame - spawnTime[i];
            float scaleFactor = (elapsed < 1.0f) ? elapsed : 1.0f;

            padoruRotation[i] += deltaTime * Speed[i];
            if (padoruRotation[i] > 360.0f) padoruRotation[i] -= 360.0f;

            model = glm::rotate(model, glm::radians(padoruRotation[i]), padoruRotationdirection[i]);
            model = glm::scale(model, glm::vec3(0.5f * scaleFactor));

            lightingShader.setMat4("model", model);
            padoruModel.Draw(lightingShader);
        }

        // Render Light Cubes
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(lightCubeVAO);

        for (int i = 0; i < 7; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("lightColor", pointLightColors[i % 4]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Callbacks
void processInput(GLFWwindow* window)
{
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (padoruCount < maxPadoru) {
            spawnTime[padoruCount] = static_cast<float>(glfwGetTime());
            padoruPositions[padoruCount] = glm::vec3(
                randomFloat(-5.0f, 5.0f),
                randomFloat(-5.0f, 5.0f),
                randomFloat(-5.0f, 5.0f)
            );
            padoruRotationdirection[padoruCount] = glm::vec3(
                randomFloat(-1.0f, 1.0f),
                randomFloat(-1.0f, 1.0f),
                randomFloat(-1.0f, 1.0f)
            );
            Speed[padoruCount] = randomFloat(-500.0f, 500.0f);
            padoruRotation[padoruCount] = 0.0f;
            padoruCount++;
        }
    }
}
