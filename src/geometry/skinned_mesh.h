#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

#include "../common.h"
#include "../utils/shader.h"

#include "mesh.h"
#include "../renderers/renderable.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

struct BoneInfo
{
    glm::mat4 BoneOffset;
    glm::mat4 FinalTransformation;        

    BoneInfo()
    {
        BoneOffset = glm::mat4(1.0f);
        FinalTransformation = glm::mat4(1.0f);          
    }
};

static const int NUM_BONES_PER_VERTEX = 4;

struct VertexBoneData
{
    uint IDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX];

    VertexBoneData() {
        for (uint i = 0 ; i < NUM_BONES_PER_VERTEX; i++) {
            IDs[i] = 0;
            Weights[i] = 0.0;
        }
    }

    void AddBoneData(uint BoneID, float Weight)
    {
        for (uint i = 0 ; i < NUM_BONES_PER_VERTEX; i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }

        // should never get here - more bones than we have space for
        assert(0);
    }
};

class SkinnedMesh: public Mesh {
public:
    /*  Mesh Data  */
    vector<VertexBoneData> bones;
    Material material;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    // SkinnedMesh(vector<Vertex> vertices, vector<VertexBoneData> bones, vector<unsigned int> indices, Material material, vector<Texture> textures);
    SkinnedMesh(const aiMesh *mesh, const aiMaterial *mat, std::vector<Texture> textures, vector<VertexBoneData> bones, glm::mat4 transform);

    // render the mesh
    void draw(Shader & shader);
    void init(Shader & shader);
    void cleanup();

private:
    unsigned int BBO; // Bone Buffer Object
};
#endif // SKINNED_MESH_H