#ifndef MESH_H
#define MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "../shader.hpp"
#include "../tiny_obj_loader.h"

using namespace std;

struct Vertex {
    // position
    glm::vec3 position;
    // normal
    glm::vec3 normal;
    // texCoords
    glm::vec2 texture_coord;
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    GLuint VAO;
    size_t material_id;

    /*  Functions  */
    // constructor
    Mesh(const vector<Vertex> vertices, const vector<unsigned int> indices, size_t material_id, mcl::Shader & shader)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->material_id = material_id;

        setup_mesh(shader);
    }

    // render the mesh
    void draw(mcl::Shader & shader, std::vector<tinyobj::material_t>& materials, std::map<std::string, GLuint>& textures) 
    {
        tinyobj::material_t mat = materials[material_id];

        // // bind appropriate textures
        // glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        if ((material_id < materials.size())) {
            tinyobj::material_t mat = materials[material_id];

            std::string diffuse_texname = mat.diffuse_texname;
            if (textures.find(diffuse_texname) != textures.end()) {
                // glUniform1i(shader.uniform("texture_map")), 0);
                glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
            }
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, indices.size());
        glBindVertexArray(0);

        // glActiveTexture(GL_TEXTURE0);
    }

private:
    /*  Render data  */
    GLuint VBO, EBO;

    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setup_mesh(mcl::Shader & shader)
    {
        GLuint attribVertexPosition  = shader.attribute("in_position");
        GLuint attribVertexNormal    = shader.attribute("in_normal");
        GLuint attribVertexColor    = shader.attribute("in_color");
        GLuint attribVertexTextureCoord     = shader.attribute("in_texture_coord");

        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(attribVertexPosition);	
        glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(attribVertexNormal);	
        glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // vertex color
        glEnableVertexAttribArray(attribVertexColor);  
        glVertexAttrib3f(attribVertexColor, 0.5, 0.2, 0.2);
        // vertex texture coords
        glEnableVertexAttribArray(attribVertexTextureCoord);	
        glVertexAttribPointer(attribVertexTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coord));

        glBindVertexArray(0);
    }
};
#endif