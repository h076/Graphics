#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "error.h"
#include "file.h"
#include "shader.h"


#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];






float vertices[] =
{
	//pos					//col			
	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	1.0f,  1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	1.0f,  1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	-1.0f,  1.0f, -1.0f,  	1.f, 0.0f, 0.0f,
	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 0.0f,

	-1.0f, -1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	1.0f, -1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	-1.0f,  1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,
	-1.0f, -1.0f,  1.0f,  	0.0f, 1.0f, 0.0f,

	-1.0f,  1.0f,  1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, -1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f,  	0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f,  1.0f,  	0.0f, 0.0f, 1.0f,

	1.0f,  1.0f,  1.0f,  	1.f, 1.0f, 0.0f,
	1.0f,  1.0f, -1.0f,  	1.f, 1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 	1.f, 1.0f, 0.0f,
	1.0f, -1.0f, -1.0f,  	1.f, 1.0f, 0.0f,
	1.0f, -1.0f,  1.0f,  	1.f, 1.0f, 0.0f,
	1.0f,  1.0f,  1.0f,  	1.f, 1.0f, 0.0f,

	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 1.0f,
	1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 1.0f,
	1.0f, -1.0f,  1.0f,  	1.f, 0.0f, 1.0f,
	1.0f, -1.0f,  1.0f,  	1.f, 0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f,  	1.f, 0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,  	1.f, 0.0f, 1.0f,

	-1.0f,  1.0f, -1.0f,  	0.0f, 1.f, 1.0f,
	1.0f,  1.0f, -1.0f,  	0.0f, 1.f, 1.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.f, 1.0f,
	1.0f,  1.0f,  1.0f,  	0.0f, 1.f, 1.0f,
	-1.0f,  1.0f,  1.0f,  	0.0f, 1.f, 1.0f,
	-1.0f,  1.0f, -1.0f, 	0.0f, 1.f, 1.0f,
};




float cam_x_offset = 0.f;
float cam_y_offset = 0.f;

float cam_xr_offset = 0.f;
float cam_yr_offset = 0.f;

void ProcessKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cam_y_offset += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cam_y_offset -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cam_x_offset += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cam_x_offset -= 0.01f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam_xr_offset -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam_xr_offset += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam_yr_offset -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam_yr_offset += 0.1f;
}

void ResizeCallback(GLFWwindow*, int w, int h)
{
	glViewport(0, 0, w, h);
}


int main(int argc, char** argv)
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "3D Transformations", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	unsigned int shaderProgram = CompileShader("triangle.vert", "triangle.frag");






	glCreateBuffers(NUM_BUFFERS, Buffers);
	glGenVertexArrays(NUM_VAOS, VAOs);

	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	glEnable(GL_DEPTH_TEST);



	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUseProgram(shaderProgram);

		glm::mat4 view = glm::mat4(1.f);
		view = glm::translate(view, -glm::vec3(0.0f+cam_x_offset, 2.f+cam_y_offset, 5.0f));
		view = glm::rotate(view, glm::radians(cam_xr_offset), glm::vec3(1, 0, 0));
		view = glm::rotate(view, glm::radians(cam_yr_offset), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), 640.f / 460.f, 1.f, 10.f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(glGetUniformLocation(shaderProgram, "uCol"), 1.f, 0.f, 0.f);
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(2.0f, 2.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUniform3f(glGetUniformLocation(shaderProgram, "uCol"), 0.f, 1.f, 0.f);
		model = glm::mat4(1.f);
		//model = glm::translate(model, glm::vec3(4.0f, 1.0f, 6.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		glfwSwapBuffers(window);

		glfwPollEvents();
		ProcessKeyboard(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}