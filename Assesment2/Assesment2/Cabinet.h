#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Trophy.h"

class Cabinet {
	public:
		Cabinet(GLuint vao, GLuint vbo, GLuint gvao, GLuint gvbo) {
			VAO = vao;
			trophy = new Trophy(vao, vbo, &vertices, 36);

			glassProgram = CompileShader("multiLight.vert", "multiLight.frag");
			glassVAO = gvao;
			glassVBO = gvbo;

			glBindVertexArray(glassVAO);
			glBindBuffer(GL_ARRAY_BUFFER, glassVBO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(glassVertices), glassVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
			glEnableVertexAttribArray(3);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void drawSet(SCamera camera, GLuint shader) {
			drawSolid(camera, shader, 10.f, 30.f, 2, false);
			drawSolid(camera, shader, 0.f, 30.f, 20, false);
			drawSolid(camera, shader, -10.f, 30.f, 200, true);
			drawTrans(camera, shader, 10.f, 30.f);
			drawTrans(camera, shader, 0.f, 30.f);
			drawTrans(camera, shader, -10.f, 30.f);
		}

		void drawSolid(SCamera camera, GLuint shader, float globalX, float globalZ, int tLayers, bool curve) {
			glUseProgram(shader);
			glBindVertexArray(VAO);

			glDisable(GL_CULL_FACE);

			glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
			glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, glm::vec3(globalX, 3.5f, globalZ));
			model = glm::scale(model, glm::vec3(5.f, 7.f, 5.f));

			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glUniform1i(glGetUniformLocation(shader, "useTexture"), false);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			trophy->draw(camera, shader, glm::vec3(globalX, 7.f, globalZ), tLayers, 35.f, 2.0f, curve);
		}

		void drawTrans(SCamera camera, GLuint shader, float globalX, float globalZ) {
			glBindVertexArray(VAO);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// draw glass
			std::vector<glm::vec3> positions;
			positions.push_back(glm::vec3(globalX, 9.5f, globalZ + 2.5f));
			positions.push_back(glm::vec3(globalX, 9.5f, globalZ - 2.5f));
			positions.push_back(glm::vec3(globalX - 2.5f, 9.5f, globalZ));
			positions.push_back(glm::vec3(globalX + 2.5f, 9.5f, globalZ));
			positions.push_back(glm::vec3(globalX, 12.f, globalZ));

			std::map<float, glm::vec3> sorted_positions;
			for (int i = 0; i < positions.size(); i++) {
				glm::vec3 v = camera.Position - positions[i];
				float l = glm::length(v);
				sorted_positions[l] = positions[i];
			}


			glDisable(GL_CULL_FACE);
			glUseProgram(glassProgram);
			glBindVertexArray(glassVAO);

			glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
			glUniformMatrix4fv(glGetUniformLocation(glassProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
			glUniformMatrix4fv(glGetUniformLocation(glassProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glm::mat4 model;
			for (auto it = sorted_positions.rbegin(); it != sorted_positions.rend(); it++) {
				model = glm::mat4(1.f);
				model = glm::translate(model, it->second);
				if (it->second.y == 12.f)
					model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
				else if (it->second.z == globalZ)
					model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
				model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
				glUniformMatrix4fv(glGetUniformLocation(glassProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			glEnable(GL_CULL_FACE);

			// unbind
			glBindVertexArray(0);
		}

		void draw(SCamera camera, GLuint shader, float globalX, float globalZ, int tLayers, bool curve) {
			glUseProgram(shader);
			glBindVertexArray(VAO);

			glDisable(GL_CULL_FACE);

			glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
			glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, glm::vec3(globalX, 3.5f, globalZ));
			model = glm::scale(model, glm::vec3(5.f, 7.f, 5.f));
			
			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glUniform1i(glGetUniformLocation(shader, "useTexture"), false);

			glDrawArrays(GL_TRIANGLES, 0, 36);
			
			trophy->draw(camera, shader, glm::vec3(globalX, 7.f, globalZ), tLayers, 35.f, 2.0f, curve);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// draw glass
			std::vector<glm::vec3> positions;
			positions.push_back(glm::vec3(globalX, 9.5f, globalZ + 2.5f));
			positions.push_back(glm::vec3(globalX, 9.5f, globalZ - 2.5f));
			positions.push_back(glm::vec3(globalX - 2.5f, 9.5f, globalZ));
			positions.push_back(glm::vec3(globalX + 2.5f, 9.5f, globalZ));
			positions.push_back(glm::vec3(globalX, 12.f, globalZ));

			std::map<float, glm::vec3> sorted_positions;
			for (int i = 0; i < positions.size(); i++) {
				glm::vec3 v = camera.Position - positions[i];
				float l = glm::length(v);
				sorted_positions[l] = positions[i];
			}

			
			glDisable(GL_CULL_FACE);
			glUseProgram(glassProgram);
			glBindVertexArray(glassVAO);

			view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
			glUniformMatrix4fv(glGetUniformLocation(glassProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

			projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
			glUniformMatrix4fv(glGetUniformLocation(glassProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


			for (auto it = sorted_positions.rbegin(); it != sorted_positions.rend(); it++) {
				model = glm::mat4(1.f);
				model = glm::translate(model, it->second);
				if (it->second.y == 12.f)
					model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
				else if(it->second.z == globalZ)
					model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
				model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
				glUniformMatrix4fv(glGetUniformLocation(glassProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			glEnable(GL_CULL_FACE);

			// unbind
			glBindVertexArray(0);
		}

	private:
		std::vector<float> vertices =
		{
			//back face
			//pos					//col				//normal
			-0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, -1.f,
			0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, -1.f,
			0.5f,  0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, -1.f,
			0.5f,  0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, -1.f,
			-0.5f,  0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, -1.f,
			-0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, -1.f,

			//front face
			-0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, 1.f,
			0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, 1.f,
			0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, 1.f,
			0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, 1.f,
			-0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, 1.f,
			-0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 0.f, 1.f,

			//left face
			-0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		-1.f, 0.f, 0.f,
			-0.5f,  0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		-1.f, 0.f, 0.f,
			-0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		-1.f, 0.f, 0.f,
			-0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		-1.f, 0.f, 0.f,
			-0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		-1.f, 0.f, 0.f,
			-0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		-1.f, 0.f, 0.f,

			//right face
			0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		1.f, 0.f, 0.f,
			0.5f,  0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		1.f, 0.f, 0.f,
			0.5f, -0.5f, -0.5f, 	0.671f, 0.545f, 0.416f,		1.f, 0.f, 0.f,
			0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		1.f, 0.f, 0.f,
			0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		1.f, 0.f, 0.f,
			0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		1.f, 0.f, 0.f,

			//bottom face
			-0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, -1.f, 0.f,
			0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, -1.f, 0.f,
			0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, -1.f, 0.f,
			0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, -1.f, 0.f,
			-0.5f, -0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, -1.f, 0.f,
			-0.5f, -0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, -1.f, 0.f,

			//top face
			-0.5f,  0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 1.f, 0.f,
			0.5f,  0.5f, -0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 1.f, 0.f,
			0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 1.f, 0.f,
			0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 1.f, 0.f,
			-0.5f,  0.5f,  0.5f,  	0.671f, 0.545f, 0.416f,		0.f, 1.f, 0.f,
			-0.5f,  0.5f, -0.5f, 	0.671f, 0.545f, 0.416f,		0.f, 1.f, 0.f
		};

		GLuint VAO;
		Trophy* trophy;

		float glassVertices[42] = {
			-0.5f, -0.5f, 0.0f,		0.659f, 0.8f, 0.843f,	0.9f,
			0.5f, -0.5f, 0.0f,		0.659f, 0.8f, 0.843f,	0.9f,
			0.5f,  0.5f, 0.0f,		0.659f, 0.8f, 0.843f,	0.9f,
			-0.5f, -0.5f, 0.0f,		0.659f, 0.8f, 0.843f,	0.9f,
			0.5f,  0.5f, 0.0f,		0.659f, 0.8f, 0.843f,	0.9f,
			-0.5f,  0.5f, 0.0f,		0.659f, 0.8f, 0.843f,	0.9f
		};


		GLuint glassVAO;
		GLuint glassVBO;
		GLuint glassProgram;
};