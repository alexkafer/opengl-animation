#include "fluid.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/normal.hpp>

static const float dx = 0.1f;

Fluid::Fluid(size_t x_dim, size_t z_dim) {
    t = 0.0f;
    _x_dim = x_dim;
    _z_dim = z_dim;
    points = std::vector<FluidPoint>();
    points.reserve(x_dim * z_dim);

    x_momentums = std::vector<float>();
    x_momentums.reserve(x_dim * z_dim);

    x_midpoint_heights = std::vector<float>();
    x_midpoint_heights.reserve(x_dim * z_dim);

    x_midpoint_momentums = std::vector<float>();
    x_midpoint_momentums.reserve(x_dim * z_dim);

    indices = std::vector<GLushort>();

    vertex = new size_t[x_dim*z_dim];

    for (size_t x = 0; x < x_dim; x++) {
        for (size_t z = 0; z < z_dim; z++) {
            vertex[x][z] = points.size();
            
            points.push_back(FluidPoint(glm::vec3(x, 0.0f, z)));
            x_momentums.push_back(0);
            x_midpoint_heights.push_back(0);
            x_midpoint_momentums.push_back(0);
        }
    }


    for (size_t x = 0; x < x_dim; x++) {
        for (size_t z = 0; z < z_dim; z++) {

            // Left down triangle
            if (x < x_dim-1 && z < z_dim-1) {
                indices.push_back(vertex[x][z]);
                indices.push_back(vertex[x+1][z]);
                indices.push_back(vertex[x][z+1]);

                // std::cout << "Made triangle " << vertex[x][y] << ", " <<vertex[x+1][y] << ", " <<vertex[x][y+1] << std::endl;
            }

            // Up right triangle
            if (x > 0 && z > 0) {
                indices.push_back(vertex[x][z]);
                indices.push_back(vertex[x-1][z]);
                indices.push_back(vertex[x][z-1]);

                // std::cout << "Made triangle " << vertex[x][y] << ", " <<vertex[x-1][y] << ", " <<vertex[x][y-1] << std::endl;
            
            }
        }
    }

    init();
}

void Fluid::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/fluid.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    glGenVertexArrays(1, &vao); //Create a VAO

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(FluidPoint), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  
    glBindVertexArray(0);

    init_static_uniforms();
}


// ///////////////////////////////////////////////////////////////////////////////
// // set uniform constants
// ///////////////////////////////////////////////////////////////////////////////
// void Fluid::init_static_uniforms()
// {
//     shader.enable();
//     GLint uniformLightPosition             = shader.uniform("lightPosition");
//     // GLint uniformLightAmbient              = shader.uniform("lightAmbient");
//     // GLint uniformLightDiffuse              = shader.uniform("lightDiffuse");
//     GLint uniformLightSpecular             = shader.uniform("lightSpecular");
//     GLint uniformMaterialAmbient           = shader.uniform("materialAmbient");
//     GLint uniformMaterialDiffuse           = shader.uniform("materialDiffuse");
//     GLint uniformMaterialSpecular          = shader.uniform("materialSpecular");
//     GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    
//     // set uniform values
//     float lightPosition[]  = {0.f, 5.0f, -3.f, 1.0f};
//     // float lightAmbient[]  = {0.3f, 0.1f, 0.1f, 1};
//     // float lightDiffuse[]  = {0.7f, 0.2f, 0.2f, 1};
//     float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
//     float materialAmbient[]  = {0.4f, 0.4f, 0.4f, 1};
//     float materialDiffuse[]  = {0.5f, 0.5f, 0.5f, 1};
//     float materialSpecular[] = {0.4f, 0.4f, 0.4f, 1};
//     float materialShininess  = 4;

//     glUniform4fv(uniformLightPosition, 1, lightPosition);
//     // glUniform4fv(uniformLightAmbient, 1, lightAmbient);
//     // glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
//     glUniform4fv(uniformLightSpecular, 1, lightSpecular);
//     glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
//     glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
//     glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
//     glUniform1f(uniformMaterialShininess, materialShininess);
// }

void Fluid::update(float dt) {
    t += dt;
    // for (size_t i = 0; i < heights.size() - 1; i++) {
    //     heights[i].position.y = M_PI + sin(t + i);
    // }

    float g = -1.f;

    // // compute halfstep for midpoints in the x direction
    for (size_t x = 0; x < _x_dim - 1; x++) {
        for (size_t z = 0; z < _z_dim - 1; z++) {
            x_midpoint_heights[vertex[x][z]] = ((heights[vertex[x][z]].position.y + heights[vertex[x+1][z]].position.y)/2.0f) 
                - (dt/2.0f)*(x_momentums[vertex[x+1][z]]-x_momentums[vertex[x][z]])/dx;

            x_midpoint_momentums[vertex[x][z]] = ((x_momentums[vertex[x][z]]+x_momentums[vertex[x+1][z]])/2.0) 
                - (dt/2.0) * 
                (sqrt(x_momentums[vertex[x+1][z]])/heights[vertex[x+1][z]].position.y + .5f*g*sqrt(heights[vertex[x+1][z]].position.y) -
                sqrt(x_momentums[vertex[x][z]])/heights[vertex[x][z]].position.y - .5f*g*sqrt(heights[vertex[x][z]].position.y))/dx;
        }
    }
        
    // then fullstep
    for (size_t x = 0; x < _x_dim - 2; x++) {
        for (size_t z = 0; z < _z_dim - 2; z++) {
            heights[vertex[x+1][z]].position.y -= dt*(x_midpoint_momentums[vertex[x+1][z]]-x_midpoint_momentums[vertex[x][z]])/dx;
            x_momentums[vertex[x+1][z]] -= dt*(sqrt(x_midpoint_momentums[vertex[x+1][z]])/x_midpoint_heights[vertex[x+1][z]] + .5*g*sqrt(x_midpoint_heights[vertex[x+1][z]]) -
            sqrt(x_midpoint_momentums[vertex[x][z]])/x_midpoint_heights[vertex[x][z]] - .5*g*sqrt(x_midpoint_heights[vertex[x][z]]))/dx;

            std::cout << "Height: " << heights[i].position.y << std::endl;
        }
    }
}

void Fluid::draw() {
    shader.enable();

    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");

	GLint attribUniformModel    = shader.uniform("model");
	GLint attribUniformNormal    = shader.uniform("normal");

    glBindVertexArray(vao);

    glVertexAttrib3f(attribVertexColor, 0.1, 0.3, 0.8);

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
   
    // glm::mat4 fluid_model = glm::translate(  // Scale first
    //     glm::mat4( 1.0f ),              // Translate second
    //     glm::vec3( 0.0f, 2.0f, 0.0f )
    // );
    glm::mat4 fluid_model = glm::mat4( 1.0f );

    glm::mat4 matrix_fluid_normal = fluid_model;
    matrix_fluid_normal[3] = glm::vec4(0,0,0,1);

    compute_normals();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(FluidPoint), &points[0], GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(FluidPoint), (void*) 0);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(FluidPoint), (GLvoid*)offsetof(FluidPoint, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(fluid_model)  ); // model transformation
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_fluid_normal)); // projection matrix

    // draw the cloth
    glDrawElements(GL_TRIANGLES,                    // primitive type
                indices.size(),          // # of indices
                GL_UNSIGNED_SHORT,                 // data type
                (void*)0);                       // offset to indices

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
    glBindVertexArray(0);

    shader.disable();
}


void Fluid::compute_normals() {
    for(size_t i = 0; i < points.size(); i++) {
        points[i].normal = glm::vec3(0.f);
    }

   for (size_t f = 0; f < indices.size() / 3; f++) {
        // Get the three indexes of the face (all faces are triangular)
        GLushort idx0 = indices[3 * f + 0];
        GLushort idx1 = indices[3 * f + 1];
        GLushort idx2 = indices[3 * f + 2];

        glm::vec3 normal = glm::triangleNormal(points[idx0].position, points[idx1].position, points[idx2].position);

        points[idx0].normal += normal;
        points[idx1].normal += normal;
        points[idx2].normal += normal;

    } 

    for(size_t i = 0; i < points.size(); i++) {
        points[i].normal = glm::normalize(points[i].normal);
    }
}


void Fluid::cleanup() {
    glDeleteVertexArrays(1, &vao);
}