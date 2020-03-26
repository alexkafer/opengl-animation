#include "../scene.h"

#include "model.h"
#include <glm/gtc/type_ptr.hpp>

#include "../utils/stb_image.h"

// https://github.com/capnramses/antons_opengl_tutorials_book/blob/master/30_skinning_part_one/main.cpp
/*  Functions   */
// constructor, expects a filepath to a 3D model.
Model::Model(string const &path, bool gamma = false) : gamma_correction(gamma)
{
    m_NumBones = 0;
    load_model(path);
}

void Model::load_model(string const &path) {
    // read file via ASSIMP
    scene = importer.ReadFile(path, aiProcess_Triangulate           |
                                    aiProcess_LimitBoneWeights      |
                                    aiProcess_GenUVCoords           |          
                                    aiProcess_GenSmoothNormals      |
                                    aiProcess_OptimizeMeshes        |
                                    aiProcess_OptimizeGraph         |
                                    aiProcess_FixInfacingNormals    |
                                    aiProcess_SortByPType           
                                    // aiProcess_PreTransformVertices
    );
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
 
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    m_GlobalInverseTransform = glm::inverse(aiMatrix4x4ToGlm(&scene->mRootNode->mTransformation));

    if (scene->HasAnimations()) {
        cout << "Scene has " << scene->mNumAnimations << " animations!" << endl;
    }

    // if (scene->HasLights()) {
    //     cout << "Scene has " << scene->mNumLights << " lights!" << endl;

    //     for(unsigned int i = 0; i < scene->mNumLights; i++) {
    //         aiLight * light = scene->mLights[i];
    //         Globals::scene->add_light(
    //             glm::vec4(light->mPosition.x,light->mPosition.y,light->mPosition.z, 1.0), 
    //             glm::vec3(light->mPosition.x,light->mPosition.y,light->mPosition.z), light->mAttenuationLinear, 0.05f);
    //     }
    // }

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, this);
}

// inits the model
void Model::init(Shader & shader) {}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, Renderable * parent)
{
    Renderable * node_placeholder = new DummyRenderable();
    node_placeholder->set_transformation(aiMatrix4x4ToGlm(&node->mTransformation));
    Globals::scene->add_renderable(node_placeholder, parent);

    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        // Mesh * mesh = ;
        Globals::scene->add_renderable(processMesh(scene->mMeshes[node->mMeshes[i]]), node_placeholder);
    }
    // after we've processed all of the meshes (if any) wqe then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], node_placeholder);
    }
}

Mesh * Model::processMesh(aiMesh * mesh) {
    // std::cout << "Loaded " << name.C_Str() << std::endl;
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];  

    vector<Texture> textures;
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

    if (mesh->HasBones()) {
        std::cout << "Model has bones" << std::endl;
        return new SkinnedMesh(mesh, material, textures, loadBones(mesh));
    } else {
        return new Mesh(mesh, material, textures);
    }
}

vector<VertexBoneData> Model::loadBones(const aiMesh* mesh) {
    std::cout << "Loading bones" << std::endl;

    vector<VertexBoneData> bones;
    bones.resize(mesh->mNumVertices);

    for (uint i = 0 ; i < mesh->mNumBones ; i++) {                
        uint BoneIndex = 0;        
        string BoneName(mesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;            
	        BoneInfo bi;			
			m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(&mesh->mBones[i]->mOffsetMatrix);            
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = m_BoneMapping[BoneName];
        }
                    
        
        for (uint j = 0 ; j < mesh->mBones[i]->mNumWeights ; j++) {
            uint VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = mesh->mBones[i]->mWeights[j].mWeight;

            bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }  

    return bones; 
}

std::string CreateBoneUniform(size_t boneIndex) {
    std::ostringstream ss;
    ss << "bones[" << boneIndex << "]";
    return ss.str();
}

void Model::update_animation(float time) {
    float ticks_per_second = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float time_in_ticks = time * ticks_per_second;
    float animation_time = fmod(time_in_ticks, (float)scene->mAnimations[0]->mDuration);

    // std::cout << "ticks_per_second: " << ticks_per_second << std::endl;
    // std::cout << "time_in_ticks: " << time_in_ticks << std::endl;
    // std::cout << "animation_time: " << animation_time << " out of " << (float)scene->mAnimations[0]->mDuration << std::endl;

    calculate_animation(animation_time, scene->mRootNode, glm::mat4(1.f));
}

uint Model::find_position(float time, const aiNodeAnim* pNodeAnim)
{    
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (time < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    std::cout << "Unable to find next position keyframe" << std::endl;

    return 0;
}


uint Model::find_rotation(float time, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (time < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
    
    std::cout << "Unable to find next rotation keyframe" << std::endl;

    return 0;
}


uint Model::find_scaling(float time, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        
        if (time < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    std::cout << "Unable to find next scaling keyframe" << std::endl;

    return 0;
}

glm::quat Model::calculate_rotation(float time, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumRotationKeys == 1) {
        return aiQuaternionToGlm(&pNodeAnim->mRotationKeys[0].mValue);
    }

    uint keyframe = find_rotation(time, pNodeAnim);
    uint next_keyframe = keyframe + 1;

    assert(next_keyframe < pNodeAnim->mNumRotationKeys);
    float dt = (float)(pNodeAnim->mRotationKeys[next_keyframe].mTime - pNodeAnim->mRotationKeys[keyframe].mTime);

    float factor = (time - (float)pNodeAnim->mRotationKeys[keyframe].mTime) / dt;

    glm::quat start = aiQuaternionToGlm(&pNodeAnim->mRotationKeys[keyframe].mValue);
    if(factor >= 0.0f && factor <= 1.0f) {
        glm::quat end = aiQuaternionToGlm(&pNodeAnim->mRotationKeys[next_keyframe].mValue);
        return glm::mix(start, end, factor);
    } else {
        return start;
    }

    
}

glm::vec3 Model::calculate_position(float time, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumPositionKeys == 1) {
        return aiVector3DToGlm(&pNodeAnim->mPositionKeys[0].mValue);
    }

    uint keyframe = find_position(time, pNodeAnim);
    uint next_keyframe = keyframe + 1;

    assert(next_keyframe < pNodeAnim->mNumPositionKeys);
    float dt = (float)(pNodeAnim->mPositionKeys[next_keyframe].mTime - pNodeAnim->mPositionKeys[keyframe].mTime);

    float factor = (time - (float)pNodeAnim->mPositionKeys[keyframe].mTime) / dt;

    glm::vec3 start = aiVector3DToGlm(&pNodeAnim->mPositionKeys[keyframe].mValue);

    if(factor >= 0.0f && factor <= 1.0f) {
        glm::vec3 end = aiVector3DToGlm(&pNodeAnim->mPositionKeys[next_keyframe].mValue);
        return glm::mix(start, end, factor);
    } else {
        return start;
    }
}

glm::vec3 Model::calculate_scaling(float time, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumScalingKeys == 1) {
        return aiVector3DToGlm(&pNodeAnim->mScalingKeys[0].mValue);
    }

    uint keyframe = find_scaling(time, pNodeAnim);
    uint next_keyframe = keyframe + 1;

    assert(next_keyframe < pNodeAnim->mNumScalingKeys);
    float dt = (float)(pNodeAnim->mScalingKeys[next_keyframe].mTime - pNodeAnim->mScalingKeys[keyframe].mTime);

    float factor = (time - (float)pNodeAnim->mScalingKeys[keyframe].mTime) / dt;
    glm::vec3 start = aiVector3DToGlm(&pNodeAnim->mScalingKeys[keyframe].mValue);

    if(factor >= 0.0f && factor <= 1.0f) {    
        glm::vec3 end = aiVector3DToGlm(&pNodeAnim->mScalingKeys[next_keyframe].mValue);
        return glm::mix(start, end, factor);
    } else {
        return start;
    }
}

void Model::calculate_animation(float time, const aiNode* pNode, const glm::mat4 & parent) {
    std::string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = scene->mAnimations[0];
    glm::mat4 node_transformation = aiMatrix4x4ToGlm(&pNode->mTransformation);
    
    const aiNodeAnim* pNodeAnim = find_node_animation(pAnimation, NodeName);

    if (pNodeAnim) {
        glm::vec3 scaling = calculate_scaling(time, pNodeAnim);
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scaling);
     
        // Interpolate rotation and generate rotation transformation matrix
        glm::quat rotation = calculate_rotation(time, pNodeAnim);
        glm::mat4 rotation_matrix = glm::toMat4(rotation);

        // Interpolate translation and generate translation transformation matrix
        glm::vec3 position = calculate_position(time, pNodeAnim);
        glm::mat4 translate_matrix = glm::translate( glm::mat4(1.0f), position);
        
        // Combine the above transformations
        node_transformation = translate_matrix * rotation_matrix * scale_matrix;
    }

    glm::mat4 global_transformation = parent * node_transformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * global_transformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        calculate_animation(time, pNode->mChildren[i], global_transformation);
    }
}

const aiNodeAnim* Model::find_node_animation(const aiAnimation* pAnimation, const string NodeName)
{
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        
        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    
    return NULL;
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
void Model::draw(Shader & shader) {
    for (size_t i= 0; i < m_NumBones; i++) {
        // std::cout << "Bone for: " << i << " has (" << glm::to_string(m_BoneInfo[i].FinalTransformation) << ")." << std::endl;
        glUniformMatrix4fv( shader.uniform(CreateBoneUniform(i)), 1, GL_FALSE, glm::value_ptr(m_BoneInfo[i].FinalTransformation)  ); // model transformation
    }
}

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