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
#include <glm/gtx/normal.hpp>

#include "../utils/tiny_obj_loader.h"
#include "../utils/stb_image.h"
#include "../shader.hpp"
#include "../utils/GLError.h"

typedef struct {
  GLuint vb_id;  // vertex buffer id
  int numTriangles;
  size_t material_id;
} DrawObject;

// Created by examining https://github.com/syoyo/tinyobjloader/blob/master/examples/viewer/viewer.cc
static bool FileExists(const std::string& abs_filename);

class Model 
{
public:
    /*  Model Data */
    std::map<std::string, GLuint> textures;
    std::vector<tinyobj::material_t> materials;
    std::vector<DrawObject> drawObjects;

    std::string mtl_dir;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(const std::string &directory, const std::string &obj_file, mcl::Shader & shader);

    // draws the model, and thus all its meshes
    void draw(mcl::Shader & shader);
    
private:
    // Based off of https://frame.42yeah.casa/2019/12/10/model-loading.html
    void load_model(std::string obj_file, std::string mtl_dir, mcl::Shader & shader);

    void process_mesh(tinyobj::attrib_t &attrib, tinyobj::mesh_t &mesh, std::map<int, glm::vec3> smoothVertexNormals, size_t shape_number);

    // Source: https://github.com/syoyo/tinyobjloader/blob/eba1fc037e89a593f50d670621c0dbf9882ec78d/examples/viewer/viewer.cc#L221
    // Check if `mesh_t` contains smoothing group id.
    bool hasSmoothingGroup(const tinyobj::shape_t& shape);

    // Modified from https://github.com/syoyo/tinyobjloader/blob/eba1fc037e89a593f50d670621c0dbf9882ec78d/examples/viewer/viewer.cc#L231
    void computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape,
                                std::map<int, glm::vec3>& smoothVertexNormals);
};


#endif