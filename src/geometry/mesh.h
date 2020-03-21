#ifndef MESH_H
#define MESH_H

#include "../common.h"
#include "../utils/shader.h"

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
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    Material material;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material, vector<Texture> textures);

    // render the mesh
    void draw(Shader & shader);
    void init(Shader & shader);
    void cleanup();

private:
    /*  Render data  */
    unsigned int VBO, EBO;
};
#endif // MESH_H