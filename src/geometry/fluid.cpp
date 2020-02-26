#include "fluid.h"

#include <stdlib.h>     /* srand, rand */

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>

// #define VERTEX(i,j,k) ((i)+(_x_dim+2)*(j) + (_x_dim+2)*(_y_dim+2)*(k))
#define VERTEX(i,j,k) ((i) * (_y_dim+2) * (_z_dim+2) + (j) * (_z_dim+2) + (k))

// #include "../utils/3d_fluid_solver.cpp"

/* external definitions (from solver.c) */
extern void dens_step ( int M, int N, int O, float * x, float * x0, float * u, float * v, float * w, float diff, float dt );
extern void vel_step ( int M, int N, int O, float * u, float * v,  float * w, float * u0, float * v0, float * w0, float visc, float dt );

static float diff = .001f; // diffuse
static float visc = 0.2f; // viscosity
static float force = 0.1f;  // added on keypress on an axis
static float source = 500.0f; // density



Fluid::Fluid(size_t x_dim, size_t y_dim, size_t z_dim) {
    _total_size = (x_dim+2)*(y_dim+2)*(z_dim+2);
    
    _x_dim = x_dim;
    _y_dim = y_dim;
    _z_dim = z_dim;

    next_source = -1;

    // size = (dimension + 2) * (dimension + 2);
    points = new glm::vec3 [_total_size];

    indices = std::vector<GLushort>();

    x_vel = new float [_total_size];
    y_vel = new float [_total_size];
    z_vel = new float [_total_size];
    x_vel_prev = new float[_total_size];
    y_vel_prev = new float[_total_size];
    z_vel_prev = new float[_total_size];

    dens = new float[_total_size];
    dens_prev = new float[_total_size];

    clear();

    float cell_unit = 1.f;
    for (size_t i = 0; i < x_dim + 2; i++) {
        float x = cell_unit * i;
        for (size_t j = 0; j < y_dim +2; j++) {
            float y = cell_unit * j;
            for (size_t k = 0; k < z_dim + 2; k++) {
                points[VERTEX(i, j, k)] = glm::vec3(x, y + 0.5f, cell_unit * k);
                std::cout << i << "," << j << "," << k << " (" << VERTEX(i, j, k) << ") is at " <<glm::to_string(points[ VERTEX(i, j, k)]) << std::endl;
            }
        }
    }

    for (size_t i = 1; i <= x_dim+1; i++) {
        for (size_t k = 1; k <= z_dim+1; k++) {
            // Left down triangle
            if (i <= x_dim && k <= z_dim) {
                indices.push_back(VERTEX(i,   1, k  ));
                indices.push_back(VERTEX(i+1, 1, k  ));
                indices.push_back(VERTEX(i,   1, k+1));

                // std::cout << "Made ld triangle " << i << ",_," << k ;
                // std::cout << " at "<< glm::to_string(points[VERTEX(i,   0, k  )]) << " - " ;
                // std::cout <<  glm::to_string(points[VERTEX(i+1,   0, k  )]) << " - " ;
                // std::cout <<  glm::to_string(points[VERTEX(i,   0, k+1  )]) << std::endl;
            }

            // Up right triangle
            if (i > 1 && k > 1) {
                indices.push_back(VERTEX(i,   1, k  ));
                indices.push_back(VERTEX(i-1, 1, k  ));
                indices.push_back(VERTEX(i,   1, k-1));

                //                std::cout << "Made ur triangle " << i << ",_," << k ;
                // std::cout << " at "<< glm::to_string(points[VERTEX(i,   0, k  )]) << " - " ;
                // std::cout <<  glm::to_string(points[VERTEX(i-1,   0, k  )]) << " - " ;
                // std::cout <<  glm::to_string(points[VERTEX(i,   0, k-1  )]) << std::endl;
            }
        }
    }

    // for (int i = 0; i < indices.size(); i++) {
    //     if (i % 3 == 0) std::cout << i << " (";
	// 	std::cout << indices.at(i) << ", " ;
    //     if (i % 3 == 2)  std::cout << ")"<< std::endl;
	// }

    srand(time(0));

    init();
}

void Fluid::clear() {
	for (int i=0; i<_total_size ; i++) {
		x_vel[i] = x_vel_prev[i] = y_vel[i] = y_vel_prev[i] = z_vel[i] = z_vel_prev[i] = dens[i] = dens_prev[i] = 0.f;
	}
}

void Fluid::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/fluid.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    shader.enable();

    glGenVertexArrays(1, &vao); //Create a VAO
    glGenBuffers(2, vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, _total_size * sizeof(glm::vec3), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, _total_size * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    for (int i=0; i<_total_size ; i++) {
        std::cout << i << ": " << glm::to_string(points[i]) << std::endl;
    }

    for (int i=0; i<indices.size() ; i++) {
        std::cout << " index " << i << ": " << indices[i] << std::endl;
    }


    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  
    glBindVertexArray(0);

    init_static_uniforms();
    shader.disable();
}

///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void Fluid::init_static_uniforms()
{
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

void Fluid::update_force_source(float * d, float * u, float * v, float * w ) {
    for (int i = 0; i < _total_size; i++) {
        int x = i / ((_y_dim+2)*(_z_dim+2));
        int y = (i-x*(_y_dim+2)*(_z_dim+2))/(_z_dim+2);
        int z = i - x * (_y_dim+2)*(_z_dim+2) - y * (_z_dim+2);

		d[i] = 0.0f;
        u[i] = force * (z - _z_dim / 2.f);
        v[i] = -9.f; // Gravity
        w[i] = -force * (x - _x_dim / 2.f);
	}

    if (next_source >= 0 && next_source < _total_size) {
		d[next_source] = source;
	} else {
        d[VERTEX(_x_dim/2, 0, _z_dim/2)] = source;
    }
}

void Fluid::update(float dt) {
    update_force_source(dens_prev, x_vel_prev, y_vel_prev, z_vel_prev);
    vel_step(_x_dim, _y_dim, _z_dim, x_vel, y_vel, z_vel,  x_vel_prev, y_vel_prev, z_vel_prev, visc, dt );
	dens_step(_x_dim, _y_dim, _z_dim, dens, dens_prev, x_vel, y_vel, z_vel, diff, dt );
}

void Fluid::draw() {
    GLint attribVertexPosition  = shader.attribute("in_position");
    // GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");
	GLint attribVertexDensity    = shader.attribute("in_density");

	// GLint attribUniformModel    = shader.uniform("model");
	// GLint attribUniformNormal    = shader.uniform("normal");

    glBindVertexArray(vao);

    // glVertexAttrib3f(attribVertexColor, 0.1f, 0.3f, 0.8f);
    glVertexAttrib3f(attribVertexNormal, 0.f, 1.f, 0.f);

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);
   
    // glm::mat4 fluid_model = glm::translate(  // Scale first
    //     glm::mat4( 1.0f ),              // Translate second
    //     glm::vec3( 0.0f, 2.0f, 0.0f )
    // );
    glm::mat4 fluid_model = glm::mat4( 1.0f );

    glm::mat4 matrix_fluid_normal = fluid_model;
    matrix_fluid_normal[3] = glm::vec4(0,0,0,1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);    
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0); 
    glEnableVertexAttribArray(attribVertexPosition);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, _total_size * sizeof(float), dens, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(attribVertexDensity);
    glVertexAttribPointer(attribVertexDensity, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*) 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(fluid_model)  ); // model transformation
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_fluid_normal)); // projection matrix

    // draw the fluid
    glDrawElements(GL_TRIANGLES,                    // primitive type
                indices.size(),          // # of indices
                GL_UNSIGNED_SHORT,                 // data type
                (void*)0);                       // offset to indices

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
    glBindVertexArray(0);

    shader.disable();
}

void Fluid::interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down) {
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

    if (mouse_down) {
        next_source = closest;
    }
}

void Fluid::cleanup() {
    glDeleteVertexArrays(1, &vao);

    delete [] points;
    delete [] x_vel;
    delete [] y_vel;
    delete [] z_vel;
    delete [] x_vel_prev;
    delete [] y_vel_prev;
    delete [] z_vel_prev;
    delete [] dens;
    delete [] dens_prev;
}

