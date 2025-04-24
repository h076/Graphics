#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"

class Floor {
public:
	Floor(GLuint buffer, GLuint vao) {
		VAO = vao;
		program = CompileShader("texture.vert", "texture.frag");
		texture = setup_texture("casinoCarpet.bmp");

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void drawFloor(SCamera Camera) {
		glDisable(GL_CULL_FACE);
		glUseProgram(program);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);

		glm::mat4 model = glm::mat4(1.f);
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
		model = glm::scale(model, glm::vec3(10.f, 10.f, 10.f));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		for (int x = -2; x <= 2; x++) {
			for (int y = -2; y <= 2; y++) {
				glm::mat4 model = glm::mat4(1.f);
				model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
				model = glm::scale(model, glm::vec3(10.f, 10.f, 10.f));
				model = glm::translate(model, glm::vec3((float)x*2, (float)y*2, 0));
				glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		glBindVertexArray(0);

		glEnable(GL_CULL_FACE);
	}

private:
	GLuint program;
	GLuint texture;
	GLuint VAO;

	float vertices[30] = {
		//t1				
		//pos					//tex
		-1.f, 1.f, 0.f,			0.f, 1.f,//tl
		1.f, 1.f, 0.f,			1.f, 1.f,//tr
		1.f, -1.f, 0.f,			1.f, 0.f,//br
		//t2					
		//pos					//tex
		-1.f, 1.f, 0.f,		0.f, 1.f,//tl
		1.f,  -1.f, 0.f,		1.f, 0.f,//br
		-1.f,  -1.f, 0.f,		0.f, 0.f//bl
	};
};