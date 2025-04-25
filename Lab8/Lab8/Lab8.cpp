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
#include "shadow.h"




//light direction variable here
glm::vec3 lightDirection(0.1f, -.81f, -.61f);
glm::vec3 lightPos(2.f, 6.f, 7.f);

SCamera Camera;


float vertices[] =
{
	//back face
	//pos					//col				//normal
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	-0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, 0.f, -1.f,

	//front face
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,

	//left face
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,

	//right face
	0.5f,  0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f,  0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f, -0.5f, -0.5f, 	1.f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f,  0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	1.f, 0.f, 0.f,

	//bottom face
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	-0.5f, -0.5f,  0.5f,  	1.f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	1.f, 1.0f, 1.0f,	0.f, -1.f, 0.f,

	//top face
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.f, 1.0f,	0.f, 1.f, 0.f,
	0.5f,  0.5f, -0.5f,  	1.0f, 1.f, 1.0f,	0.f, 1.f, 0.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f,	0.f, 1.f, 0.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f,	0.f, 1.f, 0.f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.f, 1.0f,	0.f, 1.f, 0.f,
	-0.5f,  0.5f, -0.5f, 	1.0f, 1.f, 1.0f,	0.f, 1.f, 0.f
};



#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1024
#define HEIGHT 768
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048


void drawFloorAndCubes(unsigned int shaderProgram)
{
	glBindVertexArray(VAOs[0]);

	//floor
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0, -3, 0));
	model = glm::scale(model, glm::vec3(100, 0.1, 100));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//cubes
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				glm::mat4 model = glm::mat4(1.f);
				model = glm::translate(model, glm::vec3(float(i * 2), float(j * 2), float(k * 2)));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}

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

void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void generateDepthMap(unsigned int shadowShaderProgram, ShadowStruct shadow, 
	glm::mat4 projectedLightSpaceMatrix) {
	glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shadowShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	drawFloorAndCubes(shadowShaderProgram);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind frame buffer
}

void renderWithShadow(unsigned int renderShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix) {
	glViewport(0, 0, WIDTH, HEIGHT);

	// clear colour buffer
	static const GLfloat bgd[]{ 0.8f, 0.8f, 0.8f, 1.f };
	glClearBufferfv(GL_COLOR, 0, bgd);
	glClear(GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(renderShaderProgram);

	glBindTexture(GL_TEXTURE_2D, shadow.Texture);

	// pass uniforms to shaders
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightColour"), 0.f, 0.f, 1.f);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// set up view and projection matrices
	glm::mat4 view = glm::mat4(1.f);
	view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	drawFloorAndCubes(renderShaderProgram);
}


int main(int argc, char** argv)
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	ShadowStruct shadow = setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT);

	GLuint program = CompileShader("phong.vert", "phong.frag");
	GLuint shadowProgram = CompileShader("shadow.vert", "shadow.frag");

	InitCamera(Camera, glm::vec3(10.f, 10.f, 10.f));


	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
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


	float lastFrame = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// set up the projected light matrix
		// projection does not require perspective as all light rays 
		// are parrallel using directional light.
		float near_plane = 1.0f, far_plane = 70.5f;
		// near is distance of nearest plane to camera, far is furthest
		glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;

		generateDepthMap(shadowProgram, shadow, projectedLightSpaceMatrix);
		//saveShadowMapToBitmap(shadow.Texture, SH_MAP_WIDTH, SH_MAP_HEIGHT);
		renderWithShadow(program, shadow, projectedLightSpaceMatrix);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window, Camera, deltaTime);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}