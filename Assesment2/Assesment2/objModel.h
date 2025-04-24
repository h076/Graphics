#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>  
#include "tiny_obj_loader.h"

class ObjModel {
	public:
		ObjModel(unsigned int vao, unsigned int vbo);
		~ObjModel();

		// Load obj and mtl files, splitting into submeshes by material
		bool load(const std::string& objPath, const std::string& baseDir);

		// Draw all submeshes, binding each submesh's texture
		void draw(unsigned int shader, const glm::vec3& camPos, 
			const glm::vec3& camFront, const glm::vec3& camUp) const;

	protected:
        struct Vertex {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
        };

        struct SubMesh {
            size_t firstVertex;   
            size_t vertexCount;   
            int materialID; 
        };

        // VBO/VAO
        GLuint VAO = 0, VBO = 0;

        // all the vertex data for every submesh, interleaved
        std::vector<Vertex> vertices;

        // description of each submesh’s range and material
        std::vector<SubMesh> submeshes;

        // one OpenGL texture per material (same indexing as tinyobj’s materials[])
        std::vector<GLuint> textures;

        // helper: load an image into an OpenGL texture, return its ID
        GLuint loadTextureFromFile(const std::string& filename, const std::string& baseDir);

        // set up VAO/VBO once all vertices are in `vertices`
        void setupMesh();

};