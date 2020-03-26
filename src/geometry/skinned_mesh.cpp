#include "skinned_mesh.h"

SkinnedMesh::SkinnedMesh(const aiMesh *mesh, const aiMaterial * mat, std::vector<Texture> textures, vector<VertexBoneData> bones)
: Mesh(mesh, mat, textures) {
    this->bones = bones;
}

// render the mesh
void SkinnedMesh::draw(Shader & shader) 
{
    // std::cout << "Skinned is getting called" << std::endl;
    shader.enable();
    check_gl_error();
    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to stream
        
        // std::cout << "Model has " << name + number << " texture" << std::endl;
        // now set the sampler to the correct texture unit
        glUniform1i(shader.uniform(name + number), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        check_gl_error();        
    }

    if (textures.size() > 0) {
        glUniform1i(shader.uniform("has_textures"), true);
    } 

    glUniform1i(shader.uniform("has_bones"), true);

    GLint uniformMaterialSpecular          = shader.uniform("materialSpecularColor");
    GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    check_gl_error();

    // set uniform values
    glUniform3fv(uniformMaterialSpecular, 1, material.specular);
    glUniform1f(uniformMaterialShininess, material.shininess);

    check_gl_error();
    
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void SkinnedMesh::init(Shader & shader)
{
    shader.enable();

    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexNormal  = shader.attribute("in_normal");
    GLint attribVertexTextureCoord  = shader.attribute("in_texture_coord");
    GLint attribVertexColor  = shader.attribute("in_color");

    GLint attribVertexBoneID  = shader.attribute("in_bone_ids");
    GLint attribVertexBoneWeight  = shader.attribute("in_bone_weights");

    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &BBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(attribVertexPosition);	
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(attribVertexNormal);	
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(attribVertexTextureCoord);	
    glVertexAttribPointer(attribVertexTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(attribVertexColor);
    glVertexAttribPointer(attribVertexColor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    // // vertex bitangent
    // glEnableVertexAttribArray(attribVertexBiTangent);
    // glVertexAttribPointer(attribVertexBiTangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glBindBuffer(GL_ARRAY_BUFFER, BBO);
    glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(VertexBoneData), &bones[0], GL_STATIC_DRAW);

    // for (size_t i = 0; i < bones.size(); i++) {
    //     std::cout << "Bone for: " << i << " has (" << bones[i].IDs[0] << ","<< bones[i].IDs[1] << ","<< bones[i].IDs[2] << ","<< bones[i].IDs[3] << ")." << std::endl;
    //     std::cout << "Their weights are (" << bones[i].Weights[0] << ","<< bones[i].Weights[1] << ","<< bones[i].Weights[2] << ","<< bones[i].Weights[3] << ")." << std::endl;
    // }

    glEnableVertexAttribArray(attribVertexBoneID);
    glVertexAttribIPointer(attribVertexBoneID, NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (void*)0);

    glEnableVertexAttribArray(attribVertexBoneWeight);
    glVertexAttribPointer(attribVertexBoneWeight, NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, Weights));
    
    glBindVertexArray(0);
}

void SkinnedMesh::cleanup() {
    Mesh::cleanup();
    glDeleteBuffers(1, &BBO);
}