#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "camera.h"
#include "SpotLight.h"
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

SCamera Camera;
glm::vec3 cameraTarget(0.f, 0.f, 0.f);

SpotLight spotLight;

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
    InitSpotLight(spotLight, glm::vec3(0.f, 30.f, 0.f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(4.f, 4.f, 4.f), 30.f);
    
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

        glUseProgram(texProgram);

        glUniform3f(glGetUniformLocation(texProgram, "lightDirection"), spotLight.Dir.x, spotLight.Dir.y, spotLight.Dir.z);
        glUniform3f(glGetUniformLocation(texProgram, "lightColour"), spotLight.Col.r, spotLight.Col.g, spotLight.Col.b);
        glUniform3f(glGetUniformLocation(texProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
        glUniform3f(glGetUniformLocation(texProgram, "lightPos"), spotLight.Pos.x, spotLight.Pos.y, spotLight.Pos.z);
        glUniform1f(glGetUniformLocation(texProgram, "cutOffAngle"), spotLight.CutOffAngle);

        floor.drawFloor(Camera, texProgram);
        table.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);
        chair.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);
        pillar.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);
        deck.draw(texProgram, Camera.Position, Camera.Front, Camera.Up);

        deck.deal(texProgram, Camera.Position, Camera.Front, Camera.Up, true);

        glUseProgram(program);

        glUniform3f(glGetUniformLocation(program, "lightDirection"), spotLight.Dir.x, spotLight.Dir.y, spotLight.Dir.z);
        glUniform3f(glGetUniformLocation(program, "lightColour"), spotLight.Col.r, spotLight.Col.g, spotLight.Col.b);
        glUniform3f(glGetUniformLocation(program, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
        glUniform3f(glGetUniformLocation(program, "lightPos"), spotLight.Pos.x, spotLight.Pos.y, spotLight.Pos.z);
        
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