#include "mesh.h"

Mesh::Mesh(const aiMesh *mesh, const aiMaterial * mat, const std::vector<Texture> textures)
{
    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);
    
    this->textures = textures;

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
        

        float transparency = 1.0f;
        mat->Get(AI_MATKEY_OPACITY, transparency); 

        aiColor3D diffuse(0.f,0.f,0.f);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
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
    mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    material.specular[0] = specular.r;
    material.specular[1] = specular.g;
    material.specular[2] = specular.b;

    // aiColor3D emission(0.f,0.f,0.f);
    // material->Get(AI_MATKEY_COLOR_EMISSIVE, emission);
    // mat.emission[0] = emission.r;
    // mat.emission[1] = emission.g;
    // mat.emission[2] = emission.b;
    // mat.specular[3] = transparency;
    
    material.shininess = 1;
    mat->Get(AI_MATKEY_SHININESS, material.shininess);
}

// render the mesh
void Mesh::draw(Shader & shader) 
{
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

    glUniform1i(shader.uniform("has_bones"), false);

    GLint uniformMaterialSpecular          = shader.uniform("materialSpecularColor");
    GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    check_gl_error();

    // set uniform values
    glUniform3fv(uniformMaterialSpecular, 1, material.specular);
    glUniform1f(uniformMaterialShininess, material.shininess);

    check_gl_error();
    
    // draw mesh
    // std::cout << "Drawing " << indices.size() << std::endl;
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::init(Shader & shader)
{
    shader.enable();

    std::cout << "Mesh init called" << std::endl;

    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexNormal  = shader.attribute("in_normal");
    GLint attribVertexTextureCoord  = shader.attribute("in_texture_coord");
    GLint attribVertexColor  = shader.attribute("in_color");

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

    glBindVertexArray(0);
}

void Mesh::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}