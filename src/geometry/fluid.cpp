#include "fluid.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>

#define VERTEX(i,j,k) ((i)+(_x_dim+2)*(j) + (_x_dim+2)*(_y_dim+2)*(k))

// #include "../utils/3d_fluid_solver.cpp"

/* external definitions (from solver.c) */
extern void dens_step ( int M, int N, int O, float * x, float * x0, float * u, float * v, float * w, float diff, float dt );
extern void vel_step ( int M, int N, int O, float * u, float * v,  float * w, float * u0, float * v0, float * w0, float visc, float dt );

static float diff = 0.0f; // diffuse
static float visc = 0.0f; // viscosity
static float force = 10.0f;  // added on keypress on an axis
static float source = 200.0f; // density
static float source_alpha =  0.05; //for displaying density


Fluid::Fluid(size_t x_dim, size_t y_dim, size_t z_dim) {
    _total_size = (x_dim+2)*(y_dim+2)*(z_dim+2);
    
    _x_dim = x_dim;
    _y_dim = y_dim;
    _z_dim = z_dim;

    // size = (dimension + 2) * (dimension + 2);
    points = new glm::vec3 [_total_size];

    x_vel = new float [_total_size];
    y_vel = new float [_total_size];
    z_vel = new float [_total_size];
    x_vel_prev = new float[_total_size];
    y_vel_prev = new float[_total_size];
    z_vel_prev = new float[_total_size];

    dens = new float[_total_size];
    dens_prev = new float[_total_size];

    clear();

    indices = std::vector<GLushort>();

    float cell_unit = 0.1f;
    for (size_t i = 0; i < x_dim; i++) {
        float x = cell_unit * i;
        for (size_t j = 0; j < y_dim; j++) {
            float y = cell_unit * j;
            for (size_t k = 0; k < z_dim; k++) {
                points[VERTEX(i, j, k)] = glm::vec3(x, y, cell_unit * k);
            }
        }
    }

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

    // glGenVertexArrays(1, &vao); //Create a VAO

    
    // glGenBuffers(2, vbo);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // glBufferData(GL_ARRAY_BUFFER, _size * sizeof(glm::vec3), points, GL_STATIC_DRAW);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    // glBufferData(GL_ARRAY_BUFFER, _size * sizeof(float), NULL, GL_DYNAMIC_DRAW);


    // glGenBuffers(1, &ibo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  
    // glBindVertexArray(0);

    // init_static_uniforms();
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

void Fluid::update_force_source(float * d, float * u, float * v, float * w ) {

    for (int i = 0; i < _total_size; i++) {
		u[i] = w[i]= d[i] = 0.0f;
        v[i] = -1.f; // Gravity
	}

    if (next_source >= 0) {
		d[next_source] = source;
		next_source = -1;
	}
}

void Fluid::update(float dt) {
    update_force_source(dens_prev, x_vel_prev, y_vel_prev, z_vel_prev);
    vel_step(_x_dim, _y_dim, _z_dim, x_vel, y_vel, z_vel,  x_vel_prev, y_vel_prev, z_vel_prev, visc, dt );
	dens_step(_x_dim, _y_dim, _z_dim, dens, dens_prev, x_vel, y_vel, z_vel, diff, dt );
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
    glBufferData(GL_ARRAY_BUFFER, _total_size * sizeof(float), dens, GL_DYNAMIC_DRAW);
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
    } else {
        // if (closest > 0) {
        //     next_force_dir = glm::vec2(closest % _x_dim, closest / _x_dim);
        // } else {
        //     next_force_dir = glm::vec2(0.f, 0.f);
        // }
        
        // next_force = closest;
    }
}

void Fluid::cleanup() {
    glDeleteVertexArrays(1, &vao);

    delete [] x_vel;
    delete [] y_vel;
    delete [] z_vel;
    delete [] x_vel_prev;
    delete [] y_vel_prev;
    delete [] z_vel_prev;
    delete [] dens;
    delete [] dens_prev;
}

