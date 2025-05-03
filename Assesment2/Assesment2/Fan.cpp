#include "Fan.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

void Fan::draw(unsigned int shader, const glm::vec3& camPos,
    const glm::vec3& camFront, const glm::vec3& camUp) const {
    glUseProgram(shader);
    glBindVertexArray(VAO);

    // set uniforms
    glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 500.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(glGetUniformLocation(shader, "useTexture"), true);

    // draw each submesh with its texture
    for (auto const& sm : submeshes) {
        GLuint tex = (sm.materialID >= 0 && sm.materialID < (int)textures.size())
            ? textures[sm.materialID]
            : 0;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(glGetUniformLocation(shader, "diffuseMap"), 0);

        glm::mat4 model = glm::mat4(1.f);
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
        model = glm::translate(model, glm::vec3(0.f, 1000.f, 0.f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLES, (GLint)sm.firstVertex, (GLsizei)sm.vertexCount);
    }

    glBindVertexArray(0);
}