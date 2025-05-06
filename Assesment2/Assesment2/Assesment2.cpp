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
#include "positionalLight.h"
#include "error.h"
#include "shader.h"
#include "PokerTable.h"
#include "Chair.h"
#include "Floor.h"
#include "Wall.h"
#include "Cabinet.h"
#include "Deck.h"
#include "pillar.h"
#include "Ceiling.h"
#include "Fan.h"
#include "Light.h"

#define NUM_BUFFERS 10
#define NUM_VAOS 10
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1920
#define HEIGHT 1080
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048

SCamera Camera;
glm::vec3 cameraTarget(0.f, 0.f, 0.f);

#define NUM_LIGHTS 5
SpotLight spotLight;
PosLight posLights[4];

bool deal = false;
bool dealt = false;
bool rtnDeal = false;

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

    // Interactive input
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (dealt == false) {
            deal = true;
        }
        else {
            deal = false;
            rtnDeal = true;
        }
    }

    if (xoff != 0.f || yoff != 0.f)
        MoveAndOrientCamera(camera, xoff, yoff);
}

void generateMultiDepthMap(unsigned int shadowShader, std::vector<ShadowStruct>& shadows, std::vector<glm::mat4>& projectedLightMatrices,
    SCamera Camera, Floor& floor, PokerTable& table, Chair& chair, Pillar& pillar, Cabinet& cabinet,
    Wall& wall, Deck& deck, Ceiling& ceiling, Light& light) {

    glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);

    glUseProgram(shadowShader);

    for (int i = 0; i < NUM_LIGHTS; i++) {
        ShadowStruct shadow = shadows[i];
        glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shadowShader);
        glUniformMatrix4fv(glGetUniformLocation(shadowShader, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightMatrices[i]));

        table.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);
        chair.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);
        deck.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);

        if (deal)
            dealt = deck.deal(shadowShader, Camera.Position, Camera.Front, Camera.Up, false);
        else if (rtnDeal)
            dealt = deck.in(shadowShader, Camera.Position, Camera.Front, Camera.Up, false);

        light.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);
        pillar.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);
        ceiling.draw(shadowShader, Camera.Position, Camera.Front, Camera.Up);
        cabinet.drawSet(Camera, shadowShader);
        wall.draw(Camera, shadowShader);
        floor.drawFloor(Camera, shadowShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void renderMultiLight(unsigned int renderShader, SCamera Camera, Floor& floor, PokerTable& table, 
    Chair& chair, Pillar& pillar, Cabinet& cabinet, Wall &wall, Deck& deck, Ceiling& ceiling, Light& light) {
    // clear colour buffer
    static const GLfloat bgd[]{ 0.8f, 0.8f, 0.8f, 1.f };
    glClearBufferfv(GL_COLOR, 0, bgd);
    glClear(GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(renderShader);

    // set light uniforms
    glUniform3fv(glGetUniformLocation(renderShader, "uSpotLight.Pos"), 1, glm::value_ptr(spotLight.Pos));
    glUniform3fv(glGetUniformLocation(renderShader, "uSpotLight.Dir"), 1, glm::value_ptr(spotLight.Dir));
    glUniform3fv(glGetUniformLocation(renderShader, "uSpotLight.Col"), 1, glm::value_ptr(spotLight.Col));
    glUniform1f(glGetUniformLocation(renderShader, "uSpotLight.CutOffAngle"), spotLight.CutOffAngle);

    for (int i = 0; i < 4; i++) {
        const auto& L = posLights[i];
        std::string base = "uPosLights[" + std::to_string(i) + "]";
        glUniform3fv(glGetUniformLocation(renderShader, (base + ".Pos").c_str()), 1, glm::value_ptr(L.Pos));
        glUniform3fv(glGetUniformLocation(renderShader, (base + ".Dir").c_str()), 1, glm::value_ptr(L.Dir));
        glUniform3fv(glGetUniformLocation(renderShader, (base + ".Col").c_str()), 1, glm::value_ptr(L.Col));
    }

    glUniform3f(glGetUniformLocation(renderShader, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

    table.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);
    chair.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);
    deck.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);

    if (deal)
        deck.deal(renderShader, Camera.Position, Camera.Front, Camera.Up, true);
    else if (rtnDeal)
        dealt = deck.in(renderShader, Camera.Position, Camera.Front, Camera.Up, true);

    light.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);
    pillar.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);
    ceiling.draw(renderShader, Camera.Position, Camera.Front, Camera.Up);
    floor.drawFloor(Camera, renderShader);
    wall.draw(Camera, renderShader);
    cabinet.drawSet(Camera, renderShader);
}

void renderWithMultiShadows(unsigned int renderShader, std::vector<ShadowStruct>& shadows, std::vector<glm::mat4>& projectedLightMatrices,
    SCamera Camera, Floor& floor, PokerTable& table, Chair& chair, Pillar& pillar, Cabinet& cabinet, 
    Wall& wall, Deck& deck, Ceiling& ceiling, Light& light) {

    glViewport(0, 0, WIDTH, HEIGHT);

    // clear colour buffer
    static const GLfloat bgd[]{ 0.8f, 0.8f, 0.8f, 1.f };
    glClearBufferfv(GL_COLOR, 0, bgd);
    glClear(GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(renderShader);

    // Bind each shadow map to a texture unit
    GLuint baseUnit = 1;
    for (int i = 0; i < NUM_LIGHTS; i++) {
        glActiveTexture(GL_TEXTURE0 + baseUnit + i);
        glBindTexture(GL_TEXTURE_2D, shadows[i].Texture);
    }

    // Which units carry which texturtes
    GLint loc = glGetUniformLocation(renderShader, "shadowMaps");
    std::vector<GLint> units(NUM_LIGHTS);
    for (int i = 0; i < NUM_LIGHTS; i++)
        units[i] = baseUnit + i;
    glUniform1iv(loc, NUM_LIGHTS, units.data());

    // Upload all light space matrices
    for (int i = 0; i < NUM_LIGHTS; i++) {
        std::string name = "projectedLightSpaceMatrix[" + std::to_string(i) + "]";
        GLint locM = glGetUniformLocation(renderShader, name.c_str());
        glUniformMatrix4fv(locM, 1, GL_FALSE, glm::value_ptr(projectedLightMatrices[i]));
    }

    // handle other uniforms and draw
    renderMultiLight(renderShader, Camera, floor, table, chair, pillar, cabinet, wall, deck, ceiling, light);
}


std::vector<glm::mat4> setUpProjectedLightMatrices() {
    // set up projected light matrix for each light
    // starting with all positional lights then spot light
    std::vector<glm::mat4> projectedLightMatrices;

    glm::vec3 lightDir = normalize(posLights[0].Dir);
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    glm::vec3 lightUp;

    // if lightDir is nearly parallel to worldUp, pick a different axis
    if (fabs(glm::dot(lightDir, worldUp)) > 0.99f) {
        lightUp = glm::vec3(1, 0, 0);
    }
    else {
        lightUp = worldUp;
    }

    float near_plane = 1.0f, far_plane = 70.5f;
    glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(posLights[0].Pos, posLights[0].Pos + posLights[0].Dir, lightUp);
    projectedLightMatrices.push_back(lightProjection * lightView);

    lightDir = normalize(posLights[1].Dir);
    // if lightDir is nearly parallel to worldUp, pick a different axis
    if (fabs(glm::dot(lightDir, worldUp)) > 0.99f) {
        lightUp = glm::vec3(1, 0, 0);
    }
    else {
        lightUp = worldUp;
    }

    lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
    lightView = glm::lookAt(posLights[1].Pos, posLights[1].Pos + posLights[1].Dir, lightUp);
    projectedLightMatrices.push_back(lightProjection * lightView);

    lightDir = normalize(posLights[2].Dir);
    // if lightDir is nearly parallel to worldUp, pick a different axis
    if (fabs(glm::dot(lightDir, worldUp)) > 0.99f) {
        lightUp = glm::vec3(1, 0, 0);
    }
    else {
        lightUp = worldUp;
    }

    lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
    lightView = glm::lookAt(posLights[2].Pos, posLights[2].Pos + posLights[2].Dir, lightUp);
    projectedLightMatrices.push_back(lightProjection * lightView);

    lightDir = normalize(posLights[3].Dir);
    // if lightDir is nearly parallel to worldUp, pick a different axis
    if (fabs(glm::dot(lightDir, worldUp)) > 0.99f) {
        lightUp = glm::vec3(1, 0, 0);
    }
    else {
        lightUp = worldUp;
    }

    lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
    lightView = glm::lookAt(posLights[3].Pos, posLights[3].Pos + posLights[3].Dir, lightUp);
    projectedLightMatrices.push_back(lightProjection * lightView);

    lightDir = normalize(spotLight.Dir);
    // if lightDir is nearly parallel to worldUp, pick a different axis
    if (fabs(glm::dot(lightDir, worldUp)) > 0.99f) {
        lightUp = glm::vec3(1, 0, 0);
    }
    else {
        lightUp = worldUp;
    }

    lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
    lightView = glm::lookAt(spotLight.Pos, spotLight.Pos + spotLight.Dir, lightUp);
    projectedLightMatrices.push_back(lightProjection * lightView);

    return projectedLightMatrices;
}

int main(int argc, char** argv) {
    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 32);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assesment2", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

    gl3wInit();

    //int max_samples;
    //glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
    //printf("max samples supported = %d\n", max_samples);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // one shadow map for each light
    std::vector<ShadowStruct> shadowMaps;
    for (int i = 0; i < NUM_LIGHTS; i++)
        shadowMaps.push_back(setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT));

    GLuint shadowProgram = CompileShader("shadow.vert", "shadow.frag");
    GLuint multiLightProgram = CompileShader("multiLight.vert", "multiLight.frag");

    InitCamera(Camera, glm::vec3(5.f, 10.f, 5.f));
    InitSpotLight(spotLight, glm::vec3(0.f, 30.f, 0.f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(2.f, 2.f, 2.f), 30.f);
    
    InitPosLight(posLights[0], glm::vec3(25.f, 25.f, 25.f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(5.f, 5.f, 5.f));
    InitPosLight(posLights[1], glm::vec3(-20.f, 30.f, 20.f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(5.f, 5.f, 5.f));
    InitPosLight(posLights[2], glm::vec3(20.f, 30.f, -20.f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(5.f, 5.f, 5.f));
    InitPosLight(posLights[3], glm::vec3(-20.f, 30.f, -20.f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(5.f, 5.f, 5.f));
    
    glCreateBuffers(NUM_BUFFERS, Buffers);
    glGenVertexArrays(NUM_VAOS, VAOs);

    Floor floor(Buffers[0], VAOs[0]);
    Wall wall(VAOs[7], Buffers[7]);
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

    Ceiling ceiling(VAOs[8], Buffers[8]);
    if (!ceiling.load("./objs/ceiling/Untitled.obj", "./objs/ceiling"))
        std::cout << "failed to load" << std::endl;

    Light light(VAOs[9], Buffers[9]);
    if (!light.load("./objs/light/Untitled.obj", "./objs/light"))
        std::cout << "failed to load" << std::endl;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int frameCounter = 0;

    std::vector<glm::mat4> projectedLightMatrices;

    float lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // set up projected light matrix for each light
        projectedLightMatrices = setUpProjectedLightMatrices();

        generateMultiDepthMap(shadowProgram, shadowMaps, projectedLightMatrices, Camera, 
            floor, table, chair, pillar, cabinet, wall, deck, ceiling, light);
        saveShadowMapToBitmap(shadowMaps[4].Texture, SH_MAP_WIDTH, SH_MAP_HEIGHT);
        renderWithMultiShadows(multiLightProgram, shadowMaps, projectedLightMatrices, Camera, 
            floor, table, chair, pillar, cabinet, wall, deck, ceiling, light);

        glfwSwapBuffers(window);

        glfwPollEvents();
        processKeyboard(window, Camera, deltaTime);
    }

    glDeleteVertexArrays(NUM_VAOS, VAOs);
    glDeleteBuffers(NUM_BUFFERS, Buffers);

    glfwTerminate();

    return 0;
}