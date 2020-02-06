#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "mesh.h"

#include "../tiny_obj_loader.h"
#include "../stb_image.h"
#include "../shader.hpp"


// Created by examining https://github.com/syoyo/tinyobjloader/blob/master/examples/viewer/viewer.cc
static bool FileExists(const std::string& abs_filename) {
    bool ret;
    FILE* fp = fopen(abs_filename.c_str(), "rb");
    if (fp) {
        ret = true;
        fclose(fp);
    } else {
        ret = false;
    }

    return ret;
}


class Model 
{
public:
    /*  Model Data */
    std::map<std::string, GLuint> textures;
    std::vector<tinyobj::material_t> materials;
    std::vector<Mesh> meshes;

    std::string mtl_dir;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(const std::string &directory, const std::string &obj_file, mcl::Shader & shader)
    {
        textures = std::map<std::string, GLuint>();
        meshes = std::vector<Mesh>();
        materials = std::vector<tinyobj::material_t>();
        mtl_dir = directory;

        std::stringstream model_ss; model_ss << MY_MODELS_DIR << directory;

        load_model(model_ss.str() + obj_file, model_ss.str(), shader);
    }

    // draws the model, and thus all its meshes
    void draw(mcl::Shader & shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader, materials, textures);
    }
    
private:
    /*  Functions   */

    // Based off of https://frame.42yeah.casa/2019/12/10/model-loading.html
    void load_model(std::string obj_file, std::string mtl_dir, mcl::Shader & shader) {
        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;

        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, obj_file.c_str(), mtl_dir.c_str());

        if (!warn.empty()) {
            std::cout << warn << std::endl;
        }

        if (!err.empty()) {
            std::cerr << err << std::endl;
        }

        if (!ret) {
            exit(1);
        }

        std::cout << "Loaded " << obj_file << std::endl;
        printf("# of vertices  = %d\n", (int)(attributes.vertices.size()) / 3);
        printf("# of normals   = %d\n", (int)(attributes.normals.size()) / 3);
        printf("# of texcoords = %d\n", (int)(attributes.texcoords.size()) / 2);
        printf("# of materials = %d\n", (int)materials.size());
        printf("# of shapes    = %d\n", (int)shapes.size());

        // Append `default` material
        materials.push_back(tinyobj::material_t());

        for (size_t i = 0; i < materials.size(); i++) {
            printf("material[%d].diffuse_texname = %s\n", int(i),
                materials[i].diffuse_texname.c_str());
        }

        // Load diffuse textures
        {
            for (size_t m = 0; m < materials.size(); m++) {
            tinyobj::material_t* mp = &materials[m];

            if (mp->diffuse_texname.length() > 0) {
                // Only load the texture if it is not already loaded
                if (textures.find(mp->diffuse_texname) == textures.end()) {
                    GLuint texture_id;
                    int w, h;
                    int comp;

                    std::string texture_filename = mp->diffuse_texname;
                    if (!FileExists(texture_filename)) {
                        // Append base dir.
                        texture_filename = mtl_dir + mp->diffuse_texname;
                        if (!FileExists(texture_filename)) {
                        std::cerr << "Unable to find file: " << mp->diffuse_texname
                                    << std::endl;
                        exit(1);
                        }
                    }

                    unsigned char* image =
                        stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
                    if (!image) {
                        std::cerr << "Unable to load texture: " << texture_filename
                                << std::endl;
                        exit(1);
                    }
                    std::cout << "Loaded texture: " << texture_filename << ", w = " << w
                                << ", h = " << h << ", comp = " << comp << std::endl;

                    glGenTextures(1, &texture_id);
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    if (comp == 3) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                                    GL_UNSIGNED_BYTE, image);
                    } else if (comp == 4) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, image);
                    } else {
                        assert(0);  // TODO
                    }
                    glBindTexture(GL_TEXTURE_2D, 0);
                    stbi_image_free(image);
                    textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                    }
                }
            }
        }

        for (int i = 0; i < shapes.size(); i ++) {
            tinyobj::shape_t &shape = shapes[i];
            tinyobj::mesh_t &mesh = shape.mesh;

            meshes.push_back(process_mesh(attributes, mesh, shader));
        }
    }

    Mesh process_mesh(tinyobj::attrib_t &attributes, tinyobj::mesh_t &mesh, mcl::Shader & shader)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;

        // Check for smoothing group and compute smoothing normals
        // std::map<int, vec3> smoothVertexNormals;
        // if (hasSmoothingGroup(shapes[s]) > 0) {
        //     std::cout << "Compute smoothingNormal for shape [" << s << "]" << std::endl;
        //     computeSmoothingNormals(attrib, shapes[s], smoothVertexNormals);
        // }

        // we could visit the mesh index by using mesh.indices
        for (int j = 0; j < mesh.indices.size(); j++) {
            tinyobj::index_t i = mesh.indices[j];
            glm::vec3 position = {
                attributes.vertices[i.vertex_index * 3],
                attributes.vertices[i.vertex_index * 3 + 1],
                attributes.vertices[i.vertex_index * 3 + 2]
            };
            
            glm::vec3 normal = {
                attributes.normals[i.normal_index * 3],
                attributes.normals[i.normal_index * 3 + 1],
                attributes.normals[i.normal_index * 3 + 2]
            };
            glm::vec2 texCoord = {
                attributes.texcoords[i.texcoord_index * 2],
                attributes.texcoords[i.texcoord_index * 2 + 1],
            };

            // Not gonna care about texCoord right now.
            Vertex vert = { position, normal, texCoord };
            vertices.push_back(vert);
            indices.push_back(i.vertex_index);
        }

        size_t material_id;

        // OpenGL viewer does not support texturing with per-face material.
        if (mesh.material_ids.size() > 0) {
            material_id = mesh.material_ids[0];  // use the material ID of the first face.
        } else {
            material_id = materials.size() - 1;  // = ID for default material.
        }

        printf("shape material_id %d\n", int(material_id));
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, material_id, shader);
    }
};


#endif