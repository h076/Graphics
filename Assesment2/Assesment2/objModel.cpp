#include "objModel.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

ObjModel::ObjModel(unsigned int vao, unsigned int vbo)
	: VAO(vao), VBO(vbo) {}

ObjModel::~ObjModel() {
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
	if (VBO)
		glDeleteBuffers(1, &VBO);
	for (auto t : textures)
		if (t)
			glDeleteTextures(1, &t);
}

bool ObjModel::load(const std::string& objPath, const std::string& baseDir) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, 
            objPath.c_str(), baseDir.empty() ? nullptr : baseDir.c_str());
    
    if (!warn.empty())
        std::cerr << "WARN: " << warn << "\n";
    if (!err.empty())
        std::cerr << "ERR: " << err << "\n";
    if (!ok)
        return false;

    // Load all material textures up‐front
    textures.resize(materials.size(), 0);
    for (size_t i = 0; i < materials.size(); ++i) {
        const auto& mat = materials[i];
        if (!mat.diffuse_texname.empty()) {
            std::cout << mat.diffuse_texname << std::endl;
            textures[i] = loadTextureFromFile(mat.diffuse_texname, baseDir);
            if (!textures[i]) {
                std::cerr << "Failed to load texture: " << mat.diffuse_texname << "\n";
            }
        }
    }

    // Build vertices grouped by material
    vertices.clear();
    submeshes.clear();

    struct TempRange { size_t first; size_t count; };
    std::unordered_map<int, TempRange> ranges;

    // tinyobj gives you a material_id per face
    size_t indexOffset = 0;
    for (const auto& shape : shapes) {
        size_t faceCount = shape.mesh.num_face_vertices.size();
        size_t idx = 0;
        for (size_t f = 0; f < faceCount; ++f) {
            int matID = shape.mesh.material_ids[f];  // which material for this face
            // ensure we have a range entry
            if (ranges.find(matID) == ranges.end()) {
                ranges[matID] = { indexOffset, 0 };
            }
            auto& range = ranges[matID];

            // each face is 3 vertices
            for (size_t v = 0; v < 3; ++v, ++idx, ++indexOffset) {
                const auto& idxAttrib = shape.mesh.indices[idx];
                Vertex vert{};
                vert.Position = {
                    attrib.vertices[3 * idxAttrib.vertex_index + 0],
                    attrib.vertices[3 * idxAttrib.vertex_index + 1],
                    attrib.vertices[3 * idxAttrib.vertex_index + 2]
                };
                if (idxAttrib.normal_index >= 0) {
                    vert.Normal = {
                        attrib.normals[3 * idxAttrib.normal_index + 0],
                        attrib.normals[3 * idxAttrib.normal_index + 1],
                        attrib.normals[3 * idxAttrib.normal_index + 2]
                    };
                }
                if (idxAttrib.texcoord_index >= 0) {
                    vert.TexCoords = {
                        attrib.texcoords[2 * idxAttrib.texcoord_index + 0],
                        attrib.texcoords[2 * idxAttrib.texcoord_index + 1]
                    };
                }
                vertices.push_back(vert);
                range.count++;
            }
        }
    }

    // Convert each map entry into a SubMesh
    for (auto& kv : ranges) {
        SubMesh sm;
        sm.materialID = kv.first;
        sm.firstVertex = kv.second.first;
        sm.vertexCount = kv.second.count;
        submeshes.push_back(sm);
    }

    // Upload to GPU
    setupMesh();
    return true;
}

void ObjModel::setupMesh() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
    // texcoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoords)));

    // Unbind
    glBindVertexArray(0);
}

GLuint ObjModel::loadTextureFromFile(const std::string& filename, const std::string& baseDir) {

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    std::string fullPath = baseDir.empty() ? filename : (baseDir + "/" + filename);

    std::cout << "[Texture] loading " << fullPath << "\n";

    stbi_set_flip_vertically_on_load(true);
    int w, h, n;
    unsigned char* data = stbi_load(fullPath.c_str(), &w, &h, &n, 0);
    if (!data) {
        std::cerr << "ERROR: stbi_load returned NULL for " << filename << "\n";
        return 0;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // handle different texture formats
    GLenum internalFmt, dataFmt;
    switch (n) {
    case 1: internalFmt = dataFmt = GL_RED;  break;
    case 2: internalFmt = dataFmt = GL_RG;   break;
    case 3: internalFmt = dataFmt = GL_RGB;  break;
    case 4: internalFmt = dataFmt = GL_RGBA; break;
    default:
        std::cerr << "Unexpected channel count: " << n << "\n";
        stbi_image_free(data);
        return 0;
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, w, h, 0, dataFmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    // return texture ID
    return tex;
}

void ObjModel::draw(unsigned int shader, const glm::vec3& camPos, 
    const glm::vec3& camFront, const glm::vec3& camUp) const {
    // Handle in subclasses
}

