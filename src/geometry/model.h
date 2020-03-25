#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/normal.hpp>

#include "mesh.h"
#include "skinned_mesh.h"

#include "../utils/texture.h"
#include "../utils/shader.h"
#include "../utils/GLError.h"

#include "../entities/entity.h"

GLuint TextureFromData(const GLvoid *data, int width, int height, int nrComponents, bool gamma = false);
GLuint TextureFromMemory(const aiTexture * texture, bool gamma = false);
GLuint TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model : public Renderable
{
public:
 /*  Model Data */
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
   
    std::map<string,uint> m_BoneMapping;
    uint m_NumBones;
    std::vector<BoneInfo> m_BoneInfo;

    // std::vector<Mesh> meshes;
    std::string directory;
    bool gamma_correction;

    std::string mtl_dir;
    std::string obj_file;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma);

    // draws the model, and thus all its meshes
    void init(Shader & shader);
    void draw(Shader & shader);
    void cleanup();
    
private:
    const aiScene* scene;
    // Based off of https://frame.42yeah.casa/2019/12/10/model-loading.html
    void load_model(string const &path);
    void processNode(aiNode *node);
    void processMesh(aiMesh *mesh);
    // void material_uniforms(Shader & shader, const aiMaterial material);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    vector<VertexBoneData> loadBones(const aiMesh* mesh);

    inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
    {
        glm::mat4 to;


        to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
        to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
        to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
        to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

        return to;
    }
};


#endif