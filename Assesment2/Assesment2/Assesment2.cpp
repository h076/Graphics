#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "camera.h"
#include "error.h"
#include "shader.h"
#include "PokerTable.h"
#include "Chair.h"
#include "Floor.h"
#include "Cabinet.h"
#include "Deck.h"
#include "pillar.h"

#define NUM_BUFFERS 7
#define NUM_VAOS 7
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1920
#define HEIGHT 1080

glm::vec3 lightDirection(20.f, 20.f, 0.61f);
glm::vec3 lightPos(-50.f, 15.f, -50.f);

SCamera Camera;
glm::vec3 cameraTarget(0.f, 0.f, 0.f);

void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void processKeyboard(GLFWwindow* window, SCamera& camera, float deltaTime)
{
    float velocity = camera.MovementSpeed * deltaTime;

    // Movement: W/S forward/back, A/D strafe
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.Position += camera.Front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Position -= camera.Front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Position -= camera.Right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Position += camera.Right * velocity;

    // Orientation: arrow keys to yaw/pitch
    float xoff = 0.f, yoff = 0.f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) xoff = -0.5f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) xoff = 0.5f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) yoff = 0.5f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) yoff = -0.5f;

    if (xoff != 0.f || yoff != 0.f)
        MoveAndOrientCamera(camera, xoff, yoff);
}

int main(int argc, char** argv) {
    glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assesment2", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

    gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint program = CompileShader("phong.vert", "phong.frag");
    GLuint texProgram = CompileShader("modelTex.vert", "modelTex.frag");

    InitCamera(Camera, glm::vec3(5.f, 10.f, 5.f));
    Camera.Yaw = -89.f;
    Camera.Pitch = 89.f;
    //MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f, 0.f);

    glCreateBuffers(NUM_BUFFERS, Buffers);
    glGenVertexArrays(NUM_VAOS, VAOs);

    Floor floor(Buffers[0], VAOs[0]);
    Cabinet cabinet(VAOs[3], Buffers[3], VAOs[4], Buffers[4]);

    PokerTable table(VAOs[1], Buffers[1]);
    if (!table.load("./objs/pokerTable/pokertable_round.obj", "./objs/pokerTable"))
        std::cout << "failed to load" << std::endl;

    Chair chair(VAOs[2], Buffers[2]);
    if (!chair.load("./objs/Chair/Untitled.obj", "./objs/Chair"))
        std::cout << "failed to load" << std::endl;

    Deck deck(VAOs[5], Buffers[5], 500);

    Pillar pillar(VAOs[6], Buffers[6]);
    if (!pillar.load("./objs/pillar/Untitled.obj", "./objs/Pillar"))
        std::cout << "failed to load" << std::endl;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int frameCounter = 0;

    float lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
        glClearBufferfv(GL_COLOR, 0, bgd);
        glClear(GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUseProgram(program);
        glBindVertexArray(VAOs[0]);

        glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
        glUniform3f(glGetUniformLocation(program, "lightColour"), 20.f, 20.f, 20.f);
        glUniform3f(glGetUniformLocation(program, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
        glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

        glm::mat4 view = glm::mat4(1.f);
        view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::mat4(1.f);
        projection = glm::perspective(glm::radians(60.f), (float)WIDTH / (float)HEIGHT, .1f, 500.f);
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, 0.01f, 0.f));
        model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));


        floor.drawFloor(Camera);
        table.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);
        chair.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);
        pillar.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);
        deck.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);

        deck.deal(texProgram, Camera.Position, Camera.Front, Camera.Up, true);
        
        cabinet.draw(Camera, program);

        glfwSwapBuffers(window);

        glfwPollEvents();
        processKeyboard(window, Camera, deltaTime);
    }

    glDeleteVertexArrays(NUM_VAOS, VAOs);
    glDeleteBuffers(NUM_BUFFERS, Buffers);

    glfwTerminate();

    return 0;
}