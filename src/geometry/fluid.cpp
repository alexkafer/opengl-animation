#include "fluid.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/solver.c"

#define VERTEX(i,j) ((i)+(_x_dim+2)*(j))

/* external definitions (from solver.c) */
// extern void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt );
// extern void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );

static float diff = 0.1f;
static float visc = 0.5f;

Fluid::Fluid(size_t x_dim, size_t z_dim) {
    _size = (x_dim+2)*(z_dim+2);
    _x_dim = x_dim;
    _z_dim = z_dim;
    // size = (dimension + 2) * (dimension + 2);
    points = new glm::vec3 [_size];
    u = new float [_size];
    v = new float [_size];
    u_prev = new float[_size];
    v_prev = new float[_size];
    dens = new float[_size];
    dens_prev = new float[_size];

    clear();

    indices = std::vector<GLushort>();

    float h = 1.0f / x_dim;
    float w = 1.0f / z_dim;
    for (size_t i = 0; i < x_dim; i++) {
        float x = h * i;
        for (size_t j = 0; j < z_dim; j++) {
            float z = w * j;

            points[VERTEX(i, j)] = glm::vec3(x, 0.5f, z);
            std::cout << "Placing at " << x << " and " << z << std::endl; 
        }
    }


    for (size_t x = 0; x < x_dim; x++) {
        for (size_t z = 0; z < z_dim; z++) {

            // Left down triangle IX(i,j)
            if (x < x_dim-1 && z < z_dim-1) {
                indices.push_back(VERTEX(x,z));
                indices.push_back(VERTEX(x+1,z));
                indices.push_back(VERTEX(x,z+1));

                // std::cout << "Made triangle " << vertex[x][y] << ", " <<vertex[x+1][y] << ", " <<vertex[x][y+1] << std::endl;
            }

            // Up right triangle
            if (x > 0 && z > 0) {
                indices.push_back(VERTEX(x, z));
                indices.push_back(VERTEX(x-1, z));
                indices.push_back(VERTEX(x,z-1));

                // std::cout << "Made triangle " << vertex[x][y] << ", " <<vertex[x-1][y] << ", " <<vertex[x][y-1] << std::endl;
            
            }
        }
    }

    init();
}

void Fluid::clear() {
	for (int i=0; i<_size ; i++) {
		u[i] = v[i] = u_prev[i] = v_prev[i] = dens[i] = dens_prev[i] = 0.f;
	}
}

void Fluid::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/fluid.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    glGenVertexArrays(1, &vao); //Create a VAO

    
    glGenBuffers(2, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, _size * sizeof(glm::vec3), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, _size * sizeof(float), NULL, GL_DYNAMIC_DRAW);


    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  
    glBindVertexArray(0);

    init_static_uniforms();
    std::cout << "post init" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void Fluid::init_static_uniforms()
{
    shader.enable();
    GLint uniformLightPosition             = shader.uniform("lightPosition");
    // GLint uniformLightAmbient              = shader.uniform("lightAmbient");
    // GLint uniformLightDiffuse              = shader.uniform("lightDiffuse");
    GLint uniformLightSpecular             = shader.uniform("lightSpecular");
    GLint uniformMaterialAmbient           = shader.uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader.uniform("materialDiffuse");
    GLint uniformMaterialSpecular          = shader.uniform("materialSpecular");
    GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    
    // set uniform values
    float lightPosition[]  = {0.f, 5.0f, -3.f, 1.0f};
    // float lightAmbient[]  = {0.3f, 0.1f, 0.1f, 1};
    // float lightDiffuse[]  = {0.7f, 0.2f, 0.2f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
    float materialAmbient[]  = {0.4f, 0.4f, 0.4f, 1};
    float materialDiffuse[]  = {0.5f, 0.5f, 0.5f, 1};
    float materialSpecular[] = {0.4f, 0.4f, 0.4f, 1};
    float materialShininess  = 4;

    glUniform4fv(uniformLightPosition, 1, lightPosition);
    // glUniform4fv(uniformLightAmbient, 1, lightAmbient);
    // glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
    glUniform4fv(uniformLightSpecular, 1, lightSpecular);
    glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
    glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
    glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
    glUniform1f(uniformMaterialShininess, materialShininess);
}

void Fluid::update(float dt) {
    // std::cout << "start update" << std::endl;
    

    for (int i=0 ; i<_size ; i++ ) {
		u_prev[i] = v_prev[i] = dens_prev[i] = 0.0f;
	}

    if (source > 0) {
        std::cout << "Boosting source: " << (source % _x_dim) << ", " << (source / _x_dim) << std::endl;
        u_prev[source] = 1.f;
        v_prev[source] = 1.f;

        dens_prev[source] = 1000.f;
        source = -1;
    }

    vel_step ( _x_dim, u, v, u_prev, v_prev, visc, dt );
    dens_step ( _x_dim, dens, dens_prev, u, v, diff, dt );
}

void Fluid::draw() {
    shader.enable();

    GLint attribVertexPosition  = shader.attribute("in_position");
    // GLint attribVertexColor     = shader.attribute("in_color");
    GLint attribVertexDensity     = shader.attribute("in_density");
	GLint attribVertexNormal    = shader.attribute("in_normal");

	GLint attribUniformModel    = shader.uniform("model");
	GLint attribUniformNormal    = shader.uniform("normal");

    glBindVertexArray(vao);

    // glVertexAttrib3f(attribVertexColor, 0.1, 0.3, 0.8);
    glVertexAttrib3f(attribUniformNormal, 0.f, 1.f, 0.f);

    // activate attribs
    
    // glEnableVertexAttribArray(attribVertexNormal);

    
   
    // glm::mat4 fluid_model = glm::translate(  // Scale first
    //     glm::mat4( 1.0f ),              // Translate second
    //     glm::vec3( 0.0f, 2.0f, 0.0f )
    // );
    glm::mat4 fluid_model = glm::mat4( 1.0f );

    glm::mat4 matrix_fluid_normal = fluid_model;
    matrix_fluid_normal[3] = glm::vec4(0,0,0,1);

    // compute_normals();


    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    glEnableVertexAttribArray(attribVertexPosition);
    // glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(FluidPoint), (GLvoid*)offsetof(FluidPoint, normal));


    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, _size * sizeof(float), dens, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(attribVertexDensity);
    glVertexAttribPointer(attribVertexDensity, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*) 0);

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


// void Fluid::compute_normals() {
//     for(size_t i = 0; i < points.size(); i++) {
//         points[i].normal = glm::vec3(0.f);
//     }

//    for (size_t f = 0; f < indices.size() / 3; f++) {
//         // Get the three indexes of the face (all faces are triangular)
//         GLushort idx0 = indices[3 * f + 0];
//         GLushort idx1 = indices[3 * f + 1];
//         GLushort idx2 = indices[3 * f + 2];

//         glm::vec3 normal = glm::triangleNormal(points[idx0].position, points[idx1].position, points[idx2].position);

//         points[idx0].normal += normal;
//         points[idx1].normal += normal;
//         points[idx2].normal += normal;

//     } 

//     for(size_t i = 0; i < points.size(); i++) {
//         points[i].normal = glm::normalize(points[i].normal);
//     }
// }

int Fluid::find_object(glm::vec3 origin, glm::vec3 direction) {
    float closest_distance = std::numeric_limits<float>::max();
    int closest = -1;

    glm::vec2 barry; float distance;
    for (size_t f = 0; f < indices.size() / 3; f++) {
        // Get the three indexes of the face (all faces are triangular)
        GLushort idx0 = indices[3 * f + 0];
        GLushort idx1 = indices[3 * f + 1];
        GLushort idx2 = indices[3 * f + 2];

        if (glm::intersectRayTriangle(origin, direction,
            points[idx0], points[idx1], points[idx2],
            barry, distance)) {
            if (distance < closest_distance) {
                closest = idx0;
            }
        }
    } 

    return closest;
}

void Fluid::drag_object(int object, glm::vec3 direction) {
    source = object;
}


void Fluid::cleanup() {
    glDeleteVertexArrays(1, &vao);

    delete [] u;
    delete [] v;
    delete [] u_prev;
    delete [] v_prev;
    delete [] dens;
    delete [] dens_prev;
}