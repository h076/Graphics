#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

class Trophy {
	public:
		Trophy(GLuint vao, GLuint vbo, std::vector<float>* verts, unsigned int vertexStart) {
			vertices = verts;
			generateVerts();

			VAO = vao;
			vertextStartIdx = vertexStart;
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices->size(), vertices->data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void draw(SCamera camera, GLuint shader, glm::vec3 globalTranslation) {
			//glUseProgram(shader);
			//glBindVertexArray(VAO);

			glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
			glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			// Draw all layers for the trophy base
			//glm::vec3 globalTranslation = glm::vec3(0.f, 8.f, 0.f);
			glm::mat4 model;

			for (int l = 0; l < 10; l++) {

				model = glm::mat4(1.f);
				model = glm::translate(model, globalTranslation); // move to global position

				model = glm::scale(model, glm::vec3(1.2f - (0.075f * l), 1.2f, 1.2f - (0.075f * l)));
				model = glm::translate(model, glm::vec3(0.f, (cylinderHeight / 2 * l) + (cylinderHeight / 2), 0.f));
				glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

				// draw side strip
				glDrawArrays(GL_TRIANGLE_STRIP, vertextStartIdx, sideVertexCount);

				// draw top cap
				glDrawArrays(GL_TRIANGLE_FAN, vertextStartIdx + startIdxTopCap, topCapVertexCount);
			}

			// draw sphere
			model = glm::mat4(1.f);
			model = glm::translate(model, globalTranslation); // move to global position
			model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
			model = glm::translate(model, glm::vec3(0.f, 2.8f, 0.f));
			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDisable(GL_CULL_FACE);
			glDrawArrays(GL_TRIANGLES, vertextStartIdx + sphereStartIdx, sphereVertexCount);
			glEnable(GL_CULL_FACE);

			// unbind
			//glBindVertexArray(0);
		}

	private:
		void generateVerts() {
			const float height = 0.3f;      // cylinder height
			const float radius = 1.0f;      // cylinder radius
			const int slices = 36;       // how many segments around

			cylinderHeight = height;

			// vertices of side
			float halfH = height * 0.5;
			for (int i = 0; i <= slices; i++) {
				float theta = (2.0f * M_PI * i) / slices;
				float x = cosf(theta) * radius;
				float z = sinf(theta) * radius;

				// Bottom segment
				// Pos
				vertices->push_back(x);
				vertices->push_back(-halfH);
				vertices->push_back(z);
				// Col
				vertices->push_back(0.733f);
				vertices->push_back(0.647f);
				vertices->push_back(0.239f);
				// Nor
				vertices->push_back(x / radius);
				vertices->push_back(0.0f);
				vertices->push_back(z / radius);

				// Top segment
				// Pos
				vertices->push_back(x);
				vertices->push_back(+halfH);
				vertices->push_back(z);
				// Col
				vertices->push_back(0.733f);
				vertices->push_back(0.647f);
				vertices->push_back(0.239f);
				// Nor
				vertices->push_back(x / radius);
				vertices->push_back(0.0f);
				vertices->push_back(z / radius);
			}

			sideVertexCount = (slices + 1) * 2;
			startIdxTopCap = sideVertexCount;

			// centre point of top cap
			// Pos
			vertices->push_back(0.0f);
			vertices->push_back(+halfH);
			vertices->push_back(0.0f);
			// Col
			vertices->push_back(0.733f);
			vertices->push_back(0.647f);
			vertices->push_back(0.239f);
			// Nor
			vertices->push_back(0.0f);
			vertices->push_back(1.0f);
			vertices->push_back(0.0f);

			// ring of points around the rim
			for (int i = slices; i >= 0; --i) {
				float theta = (2.0f * M_PI * i) / slices;

				float x = cosf(theta) * radius;
				float z = sinf(theta) * radius;

				// Pos
				vertices->push_back(x);
				vertices->push_back(+halfH);
				vertices->push_back(z);
				// Col
				vertices->push_back(0.733f);
				vertices->push_back(0.647f);
				vertices->push_back(0.239f);
				// Nor
				vertices->push_back(0.0f);
				vertices->push_back(1.0f);
				vertices->push_back(0.0f);
			}

			topCapVertexCount = slices + 2;

			// generate sphere vertices
			const float sphereRadius = 1.0f;
			const float stacks = 18;

			for (int i = 0; i < stacks; ++i) {
				float phi0 = M_PI * i / stacks;
				float phi1 = M_PI * (i + 1) / stacks;
				float y0 = cosf(phi0), r0 = sinf(phi0);
				float y1 = cosf(phi1), r1 = sinf(phi1);

				for (int j = 0; j < slices; ++j) {
					float t0 = 2.0f * M_PI * j / slices;
					float t1 = 2.0f * M_PI * (j + 1) / slices;
					// four corners of this “quad”
					float x00 = r0 * cosf(t0), z00 = r0 * sinf(t0);
					float x10 = r1 * cosf(t0), z10 = r1 * sinf(t0);
					float x01 = r0 * cosf(t1), z01 = r0 * sinf(t1);
					float x11 = r1 * cosf(t1), z11 = r1 * sinf(t1);

					// helper lambda to push one vertex
					auto pushVert = [&](float x, float y, float z) {
						vertices->push_back(x * sphereRadius);
						vertices->push_back(y * sphereRadius);
						vertices->push_back(z * sphereRadius);
						vertices->push_back(0.733f);
						vertices->push_back(0.647f);
						vertices->push_back(0.239f);
						vertices->push_back(x);
						vertices->push_back(y);
						vertices->push_back(z);
						};

					// triangle A
					pushVert(x00, y0, z00);
					pushVert(x10, y1, z10);
					pushVert(x11, y1, z11);

					// triangle B
					pushVert(x00, y0, z00);
					pushVert(x11, y1, z11);
					pushVert(x01, y0, z01);
				}
			}
			sphereStartIdx = sideVertexCount + topCapVertexCount;
			sphereVertexCount = stacks * slices * 6;
		}

		std::vector<float>* vertices;
		unsigned int vertextStartIdx;
		unsigned int sideVertexCount;
		unsigned int startIdxTopCap;
		unsigned int topCapVertexCount;
		unsigned int sphereStartIdx;
		unsigned int sphereVertexCount;

		float cylinderHeight;

		GLuint VAO;
};