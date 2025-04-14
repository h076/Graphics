#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"

#include "do_not_edit.h"

#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1920
#define HEIGHT 1080

glm::vec3 lightDirection(20.f, 20.f, 0.61f);
glm::vec3 lightPos(4.f, 4.f, 4.f);

SCamera Camera;


void processKeyboard(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        lightDirection = Camera.Front;
        lightPos = Camera.Position;
    }

    float x_offset = 0.f;
    float y_offset = 0.f;
    bool cam_changed = false;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        x_offset = 1.0f;
        cam_changed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        x_offset = -1.0f;
        cam_changed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        y_offset = 1.0f;
        cam_changed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        y_offset = -1.0f;
        cam_changed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        cam_dist -= 0.1f;
        cam_changed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        cam_dist += 0.1f;
        cam_changed = true;
    }

    if (cam_changed)
        MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_offset, y_offset);
}

void SizeCallback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void TrianglesToVertices(std::vector<float>* vertices, std::vector<triangle>& ts) {
    for (const triangle& t : ts) {
        // For each vertex of the triangle (v1, v2, v3):
        const vertex* vList[3] = { &t.v1, &t.v2, &t.v3 };
        for (int i = 0; i < 3; ++i) {
            const vertex& v = *vList[i];
            vertices->push_back(v.pos.x);
            vertices->push_back(v.pos.y);
            vertices->push_back(v.pos.z);
            vertices->push_back(v.col.x);
            vertices->push_back(v.col.y);
            vertices->push_back(v.col.z);
            vertices->push_back(v.nor.x);
            vertices->push_back(v.nor.y);
            vertices->push_back(v.nor.z);
        }
    }
}


int main()
{
    //const std::string MODEL_PATH = "objs/cornell2/cornell-box.obj";
    const std::string MODEL_PATH = "objs/Table.obj";

    std::vector<triangle> tris;
    obj_parse(MODEL_PATH.c_str(), &tris);
    int num_triangles = tris.size();

    std::vector<float> verticesVec;
    TrianglesToVertices(&verticesVec, tris);

    float* vertices = &verticesVec[0];

    glfwInit();

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SS8", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, SizeCallback);

    gl3wInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, 0);

    GLuint program = CompileShader("phong.vert", "phong.frag");

    InitCamera(Camera);
    cam_dist = 5.f;
    MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);

    glCreateBuffers(NUM_BUFFERS, Buffers);
    glNamedBufferStorage(Buffers[0], verticesVec.size() * sizeof(float), vertices, 0);
    glGenVertexArrays(NUM_VAOS, VAOs);
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
        glClearBufferfv(GL_COLOR, 0, bgd);
        glClear(GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUseProgram(program);

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
        model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        
        GLsizei vertexCount = static_cast<GLsizei>(verticesVec.size() / 9);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glfwSwapBuffers(window);

        glfwPollEvents();
        processKeyboard(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

