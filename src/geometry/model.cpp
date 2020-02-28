#include "../common.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include "model.h"


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


/*  Functions   */
// constructor, expects a filepath to a 3D model.
Model::Model(const std::string &directory, const std::string &obj_file, Shader & shader)
{
    textures = std::map<std::string, GLuint>();
    materials = std::vector<tinyobj::material_t>();
    mtl_dir = directory;

    std::stringstream model_ss; model_ss << MY_MODELS_DIR << directory;

    load_model(model_ss.str() + obj_file, model_ss.str(), shader);
}

// draws the model, and thus all its meshes
void Model::draw(Shader & shader)
{
    GLuint attribVertexPosition  = shader.attribute("in_position");
    GLuint attribVertexNormal    = shader.attribute("in_normal");
    GLuint attribVertexColor    = shader.attribute("in_color");
    GLuint attribVertexTextureCoord     = shader.attribute("in_texture_coord");

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glPolygonMode(GL_BACK, GL_FILL);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    
    GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
    for (size_t i = 0; i < drawObjects.size(); i++) {
        DrawObject o = drawObjects[i];
        if (o.vb_id < 1) {
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);

        glBindTexture(GL_TEXTURE_2D, 0);
        if ((o.material_id < materials.size())) {
            load_material(materials[o.material_id], shader);
        }
        
        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(attribVertexPosition);	
        glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        // vertex normals
        glEnableVertexAttribArray(attribVertexNormal);	
        glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 3));
        // vertex color
        glEnableVertexAttribArray(attribVertexColor);  
        glVertexAttribPointer(attribVertexColor, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 6));
        // vertex texture coords
        glEnableVertexAttribArray(attribVertexTextureCoord);	
        glVertexAttribPointer(attribVertexTextureCoord, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 9));


        glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
        check_gl_error();
        glBindTexture(GL_TEXTURE_2D, 0);

        glUniform1i(shader.uniform("texture_used"), 0);
    }
}

void Model::load_material(tinyobj::material_t material, Shader & shader)
{
    GLint uniformMaterialAmbient           = shader.uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader.uniform("materialDiffuse");
    GLint uniformMaterialSpecular          = shader.uniform("materialSpecular");
    GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    check_gl_error();

    std::string diffuse_texname = material.diffuse_texname;
    if (textures.find(diffuse_texname) != textures.end()) {
        glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
        glUniform1i(shader.uniform("texture_used"), 1);
    } 


    float materialAmbient[]  = {material.ambient[0], material.ambient[1], material.ambient[2], 1};
    float materialDiffuse[]  = {material.diffuse[0], material.diffuse[1], material.diffuse[2], 1};
    float materialSpecular[] = {material.specular[0], material.specular[1], material.specular[2], 1};
    float materialShininess  = material.shininess;

    glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
    glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
    glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
    glUniform1f(uniformMaterialShininess, materialShininess);

    check_gl_error();
}


// Based off of https://frame.42yeah.casa/2019/12/10/model-loading.html
void Model::load_model(std::string obj_file, std::string mtl_dir, Shader & shader) {
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

    // for (size_t i = 0; i < materials.size(); i++) {
    //     printf("material[%d].diffuse_texname = %s\n", int(i),
    //         materials[i].diffuse_texname.c_str());
    // }

    // Load diffuse textures
    {
        for (size_t m = 0; m < materials.size(); m++) {
            tinyobj::material_t* mp = &materials[m];

            if (mp->diffuse_texname.length() > 0) {
                // Only load the texture if it is not already loaded
                if (textures.find(mp->diffuse_texname) == textures.end()) {
                    GLuint texture_id;
            
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

                    texture_id = load_texture(texture_filename.c_str());
                    textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                }
            }
        }

        for (size_t i = 0; i < shapes.size(); i ++) {
            tinyobj::shape_t &shape = shapes[i];

            // Check for smoothing group and compute smoothing normals
            std::map<int, glm::vec3> smoothVertexNormals;
            if (hasSmoothingGroup(shape) > 0) {
                std::cout << "Compute smoothingNormal for shape [" << i << "]" << std::endl;
                computeSmoothingNormals(attributes, shape, smoothVertexNormals);
            }

            process_mesh(attributes, shape.mesh, smoothVertexNormals, i);
        }
    }
}

void Model::process_mesh(tinyobj::attrib_t &attrib, tinyobj::mesh_t &mesh, std::map<int, glm::vec3> smoothVertexNormals, size_t shape_number) {
    DrawObject o;
    std::vector<float> buffer;  // pos(3float), normal(3float), color(3float)

    for (size_t f = 0; f < mesh.indices.size() / 3; f++) {
        tinyobj::index_t idx0 = mesh.indices[3 * f + 0];
        tinyobj::index_t idx1 = mesh.indices[3 * f + 1];
        tinyobj::index_t idx2 = mesh.indices[3 * f + 2];

        int current_material_id = mesh.material_ids[f];

        if ((current_material_id < 0) ||
            (current_material_id >= static_cast<int>(materials.size()))) {
        // Invaid material ID. Use default material.
        current_material_id =
            materials.size() -
            1;  // Default material is added to the last item in `materials`.
        }
        // if (current_material_id >= materials.size()) {
        //    std::cerr << "Invalid material index: " << current_material_id <<
        //    std::endl;
        //}
        //
        float diffuse[3];
        for (size_t i = 0; i < 3; i++) {
        diffuse[i] = materials[current_material_id].diffuse[i];
        }
        float tc[3][2];
        if (attrib.texcoords.size() > 0) {
        if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
            (idx2.texcoord_index < 0)) {
            // face does not contain valid uv index.
            tc[0][0] = 0.0f;
            tc[0][1] = 0.0f;
            tc[1][0] = 0.0f;
            tc[1][1] = 0.0f;
            tc[2][0] = 0.0f;
            tc[2][1] = 0.0f;
        } else {
            assert(attrib.texcoords.size() >
                size_t(2 * idx0.texcoord_index + 1));
            assert(attrib.texcoords.size() >
                size_t(2 * idx1.texcoord_index + 1));
            assert(attrib.texcoords.size() >
                size_t(2 * idx2.texcoord_index + 1));

            // Flip Y coord.
            tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
            tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
            tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
            tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
            tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
            tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
        }
        } else {
            tc[0][0] = 0.0f;
            tc[0][1] = 0.0f;
            tc[1][0] = 0.0f;
            tc[1][1] = 0.0f;
            tc[2][0] = 0.0f;
            tc[2][1] = 0.0f;
        }

        glm::vec3 v[3];
        for (int k = 0; k < 3; k++) {
            int f0 = idx0.vertex_index;
            int f1 = idx1.vertex_index;
            int f2 = idx2.vertex_index;
            assert(f0 >= 0);
            assert(f1 >= 0);
            assert(f2 >= 0);

            v[0][k] = attrib.vertices[3 * f0 + k];
            v[1][k] = attrib.vertices[3 * f1 + k];
            v[2][k] = attrib.vertices[3 * f2 + k];
        }

        glm::vec3 n[3];
        {
            bool invalid_normal_index = false;
            if (attrib.normals.size() > 0) {
                int nf0 = idx0.normal_index;
                int nf1 = idx1.normal_index;
                int nf2 = idx2.normal_index;

                if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) {
                    // normal index is missing from this face.
                    invalid_normal_index = true;
                } else {
                    for (int k = 0; k < 3; k++) {
                        assert(size_t(3 * nf0 + k) < attrib.normals.size());
                        assert(size_t(3 * nf1 + k) < attrib.normals.size());
                        assert(size_t(3 * nf2 + k) < attrib.normals.size());
                        n[0][k] = attrib.normals[3 * nf0 + k];
                        n[1][k] = attrib.normals[3 * nf1 + k];
                        n[2][k] = attrib.normals[3 * nf2 + k];
                    }
                }
            } else {
                invalid_normal_index = true;
            }

            if (invalid_normal_index && !smoothVertexNormals.empty()) {
                // Use smoothing normals
                int f0 = idx0.vertex_index;
                int f1 = idx1.vertex_index;
                int f2 = idx2.vertex_index;

                if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
                    n[0] = smoothVertexNormals[f0];
                    n[1] = smoothVertexNormals[f1];
                    n[2] = smoothVertexNormals[f2];

                    invalid_normal_index = false;
                }
            }

            if (invalid_normal_index) {
                // compute geometric normal
                n[0] = glm::triangleNormal(v[0], v[1], v[2]);
                n[1] = n[0];
                n[2] = n[0];
            }
        }

        for (int k = 0; k < 3; k++) {
            buffer.push_back(v[k][0]);
            buffer.push_back(v[k][1]);
            buffer.push_back(v[k][2]);
            buffer.push_back(n[k][0]);
            buffer.push_back(n[k][1]);
            buffer.push_back(n[k][2]);
            // Combine normal and diffuse to get color.
            // float normal_factor = 0.2;
            // float diffuse_factor = 1 - normal_factor;
            // float c[3] = {n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
            //                 n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
            //                 n[k][2] * normal_factor + diffuse[2] * diffuse_factor};
            // float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
            // if (len2 > 0.0f) {
            //     float len = sqrtf(len2);

            //     c[0] /= len;
            //     c[1] /= len;
            //     c[2] /= len;
            // }
            // float c[3] = {0.7f, 0.2f, 0.2f};
            float c[3] = {1.f, 1.f, 1.f};

            buffer.push_back(c[0]);
            buffer.push_back(c[1]);
            buffer.push_back(c[2]);

            buffer.push_back(tc[k][0]);
            buffer.push_back(tc[k][1]);
        }
    }

    o.vb_id = 0;
    o.numTriangles = 0;

    // OpenGL viewer does not support texturing with per-face material.
    if (mesh.material_ids.size() > 0 &&
        mesh.material_ids.size() > shape_number) {
        o.material_id = mesh.material_ids[0];  // use the material ID
                                                        // of the first face.
    } else {
        o.material_id = materials.size() - 1;  // = ID for default material.
    }
    printf("shape[%d] material_id %d\n", int(shape_number), int(o.material_id));

    if (buffer.size() > 0) {
        glGenBuffers(1, &o.vb_id);
        glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float),
                    &buffer.at(0), GL_STATIC_DRAW);
        o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) /
                        3;  // 3:vtx, 3:normal, 3:col, 2:texcoord

        printf("shape[%d] # of triangles = %d\n", static_cast<int>(shape_number),
            o.numTriangles);
    }

    drawObjects.push_back(o);
}

// Source: https://github.com/syoyo/tinyobjloader/blob/eba1fc037e89a593f50d670621c0dbf9882ec78d/examples/viewer/viewer.cc#L221
// Check if `mesh_t` contains smoothing group id.
bool Model::hasSmoothingGroup(const tinyobj::shape_t& shape)
{
    for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++) {
        if (shape.mesh.smoothing_group_ids[i] > 0) {
            return true;
        }
    }
    return false;
}

// Modified from https://github.com/syoyo/tinyobjloader/blob/eba1fc037e89a593f50d670621c0dbf9882ec78d/examples/viewer/viewer.cc#L231
void Model::computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape,
                            std::map<int, glm::vec3>& smoothVertexNormals) {
    smoothVertexNormals.clear();
    std::map<int, glm::vec3>::iterator iter;

    for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
        // Get the three indexes of the face (all faces are triangular)
        tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
        tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
        tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

        // Get the three vertex indexes and coordinates
        int vi[3];      // indexes
        glm::vec3 v[3];  // coordinates

        for (int k = 0; k < 3; k++) {
            vi[0] = idx0.vertex_index;
            vi[1] = idx1.vertex_index;
            vi[2] = idx2.vertex_index;
            assert(vi[0] >= 0);
            assert(vi[1] >= 0);
            assert(vi[2] >= 0);

            v[0][k] = attrib.vertices[3 * vi[0] + k];
            v[1][k] = attrib.vertices[3 * vi[1] + k];
            v[2][k] = attrib.vertices[3 * vi[2] + k];
        }

        // Compute the normal of the face
        // float normal[3];
        // CalcNormal(normal, v[0], v[1], v[2]);

        glm::vec3 normal = glm::triangleNormal(v[0], v[1], v[2]);

        // Add the normal to the three vertexes
        for (size_t i = 0; i < 3; ++i) {
            iter = smoothVertexNormals.find(vi[i]);
            if (iter != smoothVertexNormals.end()) {
                // add
                iter->second += normal;
            } else {
                smoothVertexNormals[vi[i]] = normal;
            }
        }

    }  // f

    // Normalize the normals, that is, make them unit vectors
    for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end();
        iter++) {
        iter->second = glm::normalize(iter->second);
    }

}  // computeSmoothingNormals
