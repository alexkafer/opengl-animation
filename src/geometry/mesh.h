#ifndef MESH_H
#define MESH_H

#include "../common.h"
#include "../utils/shader.h"

#include "../renderers/renderable.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;


struct Vertex {
    // position
    glm::vec3 Position;
    // color
    glm::vec4 Color;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // // tangent
    // glm::vec3 Tangent;
    // // bitangent
    // glm::vec3 Bitangent;
};


struct Texture {
    unsigned int id;
    string type;
    string path;
};

struct Material {
    float specular[3];
    float shininess;

    Material() {
        specular[0] = specular[1] = specular[2] = 0.0f;
        shininess = 0.0f;
    }
};

class Mesh: public Renderable {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    Material material;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    // Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material, vector<Texture> textures);
    Mesh(const aiMesh *mesh, const aiMaterial *mat, const std::vector<Texture> textures_loaded);

    // render the mesh
    void draw(Shader & shader);
    void init(Shader & shader);
    void cleanup();

protected:
    /*  Render data  */
    unsigned int VBO, EBO;
};
#endif // MESH_H