#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "camera.h"
#include "shadow.h"
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
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048

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

void generateDepthMap(unsigned int shadowShader, ShadowStruct shadow, 
    glm::mat4 projectedLightSpaceMatrix, SCamera Camera, Floor& floor, PokerTable& table, Chair& chair) {

    glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(shadowShader);
    glUniformMatrix4fv(glGetUniformLocation(shadowShader, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
    
    table.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);
    chair.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);
    floor.drawFloor(Camera, shadowShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void renderWithShadow(unsigned int renderShader, ShadowStruct shadow,
    glm::mat4 projectedLightSpaceMatrix, SCamera Camera, Floor& floor, PokerTable& table, Chair& chair) {

    glViewport(0, 0, WIDTH, HEIGHT);

    // clear colour buffer
    static const GLfloat bgd[]{ 0.8f, 0.8f, 0.8f, 1.f };
    glClearBufferfv(GL_COLOR, 0, bgd);
    glClear(GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(renderShader);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow.Texture);

    // 2) pass that unit index into the sampler2D in your shader:
    GLint loc = glGetUniformLocation(renderShader, "shadowMap");
    glUseProgram(renderShader);
    glUniform1i(loc, 1);

    // set uniforms for all models using textures
    glUniformMatrix4fv(glGetUniformLocation(renderShader, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
    glUniform3f(glGetUniformLocation(renderShader, "lightDirection"), spotLight.Dir.x, spotLight.Dir.y, spotLight.Dir.z);
    glUniform3f(glGetUniformLocation(renderShader, "lightColour"), spotLight.Col.r, spotLight.Col.g, spotLight.Col.b);
    glUniform3f(glGetUniformLocation(renderShader, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
    glUniform3f(glGetUniformLocation(renderShader, "lightPos"), spotLight.Pos.x, spotLight.Pos.y, spotLight.Pos.z);
    glUniform1f(glGetUniformLocation(renderShader, "cutOffAngle"), spotLight.CutOffAngle);

    table.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);
    chair.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);
    floor.drawFloor(Camera, renderShader);
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

    ShadowStruct shadow = setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT);

    GLuint program = CompileShader("phong.vert", "phong.frag");
    GLuint texProgram = CompileShader("modelTex.vert", "modelTex.frag");
    GLuint shadowProgram = CompileShader("shadow.vert", "shadow.frag");

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

        glm::vec3 lightDir = normalize(spotLight.Dir);
        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        glm::vec3 lightUp;

        // if lightDir is nearly parallel to worldUp, pick a different axis
        if (fabs(glm::dot(lightDir, worldUp)) > 0.99f) {
            lightUp = glm::vec3(1, 0, 0);
        }
        else {
            lightUp = worldUp;
        }

        // set up the projected light matrix
        // projection does not require perspective as all light rays 
        // are parrallel using directional light.
        float near_plane = 1.0f, far_plane = 70.5f;
        // near is distance of nearest plane to camera, far is furthest
        glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(spotLight.Pos, spotLight.Pos + spotLight.Dir, lightUp);
        glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;

        generateDepthMap(shadowProgram, shadow, projectedLightSpaceMatrix, Camera, floor, table, chair);
        renderWithShadow(texProgram, shadow, projectedLightSpaceMatrix, Camera, floor, table, chair);

        glUseProgram(texProgram);

        // set uniforms for all models using textures
        glUniform3f(glGetUniformLocation(texProgram, "lightDirection"), spotLight.Dir.x, spotLight.Dir.y, spotLight.Dir.z);
        glUniform3f(glGetUniformLocation(texProgram, "lightColour"), spotLight.Col.r, spotLight.Col.g, spotLight.Col.b);
        glUniform3f(glGetUniformLocation(texProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
        glUniform3f(glGetUniformLocation(texProgram, "lightPos"), spotLight.Pos.x, spotLight.Pos.y, spotLight.Pos.z);
        glUniform1f(glGetUniformLocation(texProgram, "cutOffAngle"), spotLight.CutOffAngle);

        // set uniforms in deck for card shadow
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