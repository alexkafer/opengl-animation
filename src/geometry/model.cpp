#include "../scene.h"

#include "model.h"

#include "../utils/stb_image.h"
// https://github.com/capnramses/antons_opengl_tutorials_book/blob/master/30_skinning_part_one/main.cpp
/*  Functions   */
// constructor, expects a filepath to a 3D model.
Model::Model(string const &path, bool gamma = false) : gamma_correction(gamma)
{
    load_model(path);
}

void Model::load_model(string const &path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    scene = importer.ReadFile(path, aiProcess_Triangulate           |
                                    aiProcess_GenUVCoords           |
                                    aiProcess_GenSmoothNormals      |
                                    aiProcess_OptimizeMeshes        |
                                    aiProcess_OptimizeGraph         |
                                    aiProcess_FixInfacingNormals    |
                                    aiProcess_JoinIdenticalVertices |
                                    aiProcess_SortByPType           | 
                                    aiProcess_PreTransformVertices);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
 
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // std::cout << "Directory: " << directory << std::endl;

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode);
}

// draws the model, and thus all its meshes
void Model::init(Shader & shader) {}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        Mesh * mesh = processMesh(scene->mMeshes[node->mMeshes[i]]);
        Globals::scene->add_renderable(mesh, this);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i]);
    }

}

Mesh * Model::processMesh(aiMesh *mesh)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];  

        // Walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y; // Flip y and z coords
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else 
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            

            float transparency = 1;
            material->Get(AI_MATKEY_OPACITY, transparency); 

            aiColor3D diffuse(0.f,0.f,0.f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
            vertex.Color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, transparency);
            
            // // tangent
            // if (mesh->mTangents != nullptr) {
            //     vector.x = mesh->mTangents[i].x;
            //     vector.y = mesh->mTangents[i].y;
            //     vector.z = mesh->mTangents[i].z;
            //     vertex.Tangent = vector;
            // }

            // // bitangent
            // if (mesh->mBitangents != nullptr) {
            //     vector.x = mesh->mBitangents[i].x;
            //     vector.y = mesh->mBitangents[i].y;
            //     vector.z = mesh->mBitangents[i].z;
            //     vertex.Bitangent = vector;
            // }
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        

        aiString name;
        material->Get(AI_MATKEY_NAME,name);

        Material mat;


        // aiColor3D ambient(0.f,0.f,0.f);
        // material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        // mat.ambient[0] = ambient.r;
        // mat.ambient[1] = ambient.g;
        // mat.ambient[2] = ambient.b;
        // mat.ambient[3] = transparency;
        
        // aiColor3D diffuse(0.f,0.f,0.f);
        // material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        // mat.diffuse[0] = diffuse.r;
        // mat.diffuse[1] = diffuse.g;
        // mat.diffuse[2] = diffuse.b;
        // mat.diffuse[3] = transparency;

        
        
        aiColor3D specular(0.f,0.f,0.f);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        mat.specular[0] = specular.r;
        mat.specular[1] = specular.g;
        mat.specular[2] = specular.b;

        // aiColor3D emission(0.f,0.f,0.f);
        // material->Get(AI_MATKEY_COLOR_EMISSIVE, emission);
        // mat.emission[0] = emission.r;
        // mat.emission[1] = emission.g;
        // mat.emission[2] = emission.b;
        // mat.specular[3] = transparency;
        
        mat.shininess = 1;
        material->Get(AI_MATKEY_SHININESS, mat.shininess);

        // std::cout << "Loaded " << name.C_Str() << std::endl;
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN
        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return new Mesh(vertices, indices, mat, textures);
    }

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;

            if (const aiTexture * texture_ai = scene->GetEmbeddedTexture(str.C_Str())) {

                texture.id = TextureFromMemory(texture_ai);
                std::cout << "Loaded embedded texture " << str.C_Str() << " as " << texture.id << std::endl;
            } else {
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                std::cout << "Loaded file texture " << str.C_Str() << " as " << texture.id << std::endl;
            }

            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}

// draws the model, and thus all its meshes
void Model::draw(Shader & shader) {}

void Model::cleanup() {}

GLuint TextureFromData(const GLvoid *data, int width, int height, int nrComponents, bool gamma) {
    check_gl_error();

    GLuint textureID;
    glGenTextures(1, &textureID);
    
    GLenum format;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    check_gl_error();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    check_gl_error();


    return textureID;
}

GLuint TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLuint id = TextureFromData(data, width, height, nrComponents, gamma);
        stbi_image_free(data);
        return id;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return -1;
    }
}


GLuint TextureFromMemory(const aiTexture * texture, bool gamma)
{
    int width, height, nrComponents;
    unsigned char *image_data = nullptr;

    if (texture->mHeight == 0)
    {
        image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &nrComponents, 0);
    }
    else
    {
        image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &nrComponents, 0);
    }

    if (image_data)
    {
        GLuint id = TextureFromData(image_data, width, height, nrComponents, gamma);
        stbi_image_free(image_data);
        return id;
    }
    else
    {
        std::cout << "Texture failed to load embedded texture" << std::endl;
        stbi_image_free(image_data);
        return -1;
    }
}