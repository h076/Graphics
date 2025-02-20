#define _USE_MATH_DEFINES 

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "error.h"
#include "file.h"
#include "shader.h"


#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

void ResizeCallback(GLFWwindow*, int w, int h)
{
	glViewport(0, 0, w, h);
}

template<typename T>
constexpr auto DEG2RAD(T n) { return n*(M_PI/180); }

float* CreateCircle(int num_segments, float radius)
{
	float* rtn = (float*) malloc(sizeof(float) * 3 * 3 * num_segments);
	float offset = 360.f / num_segments;
	float angle = 0;
	for (int i = 0; i < num_segments; i++) {
		// v0
		*(rtn + (i * 9)) = 0;
		*(rtn + (i * 9) + 1) = 0;
		*(rtn + (i * 9) + 2) = 0;

		//v1
		*(rtn + (i * 9) + 3) = radius * sin(DEG2RAD(angle));
		*(rtn + (i * 9) + 4) = radius * cos(DEG2RAD(angle));
		*(rtn + (i * 9) + 5) = 0;

		//v2
		*(rtn + (i * 9) + 6) = radius * sin(DEG2RAD(angle + offset));
		*(rtn + (i * 9) + 7) = radius * cos(DEG2RAD(angle + offset));
		*(rtn + (i * 9) + 8) = 0;

		angle += offset;
	}
	return rtn;
}

void printCircle(float* circle, int num_segments) {
	for (int i = 0; i < num_segments; i++) {
		std::cout << "Segment " << i << ":" << std::endl;
		std::cout << "  v0: (" << circle[i * 9] << ", " << circle[i * 9 + 1] << ", " << circle[i * 9 + 2] << ")" << std::endl;
		std::cout << "  v1: (" << circle[i * 9 + 3] << ", " << circle[i * 9 + 4] << ", " << circle[i * 9 + 5] << ")" << std::endl;
		std::cout << "  v2: (" << circle[i * 9 + 6] << ", " << circle[i * 9 + 7] << ", " << circle[i * 9 + 8] << ")" << std::endl;
	}
}

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "2D modelling", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	gl3wInit();


	GLuint program = CompileShader("triangle.vert", "triangle.frag");

	int num_segments = 32;
	float* verts = CreateCircle(num_segments, 0.5f);
	//printCircle(verts, num_segments);

	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(float) * 3 * 3 * num_segments, verts, 0);

	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);
		glBindVertexArray(VAOs[0]);

		glDrawArrays(GL_TRIANGLES, 0, num_segments * 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	free(verts);

	glfwDestroyWindow(window);
	glfwTerminate();
}