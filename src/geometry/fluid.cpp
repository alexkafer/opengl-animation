#include "fluid.h"

#include "../utils/GLError.h"

#include <stdlib.h>     /* srand, rand */

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>

#include <GLFW/glfw3.h>

// #define VERTEX(i,j,k) ((i)+(_x_dim+2)*(j) + (_x_dim+2)*(_y_dim+2)*(k))
#define VERTEX(i,j,k) ((i) * (_y_dim+2) * (_z_dim+2) + (j) * (_z_dim+2) + (k))

// #include "../utils/3d_fluid_solver.cpp"

/* external definitions (from solver.c) */
extern void dens_step ( int M, int N, int O, float * x, float * x0, float * u, float * v, float * w, float diff, float dt );
extern void vel_step ( int M, int N, int O, float * u, float * v,  float * w, float * u0, float * v0, float * w0, float visc, float dt );

static float diff = 0.0002f; // diffuse
static float visc = 20.f; // viscosity
static float force = 2000.0f;
static float source = 1000.0f; // density


Fluid::Fluid(size_t x_dim, size_t y_dim, size_t z_dim) {
    _total_size = (x_dim+2)*(y_dim+2)*(z_dim+2);
    
    _x_dim = x_dim;
    _y_dim = y_dim;
    _z_dim = z_dim;

    addforce[0] = addforce[1] = addforce[2] = 0;
    addsource = 0;

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

    float cell_unit = .5f;
    for (size_t i = 0; i < x_dim + 2; i++) {
        float x = cell_unit * i;
        for (size_t j = 0; j < y_dim +2; j++) {
            float y = cell_unit * j;
            for (size_t k = 0; k < z_dim + 2; k++) {
                points[VERTEX(i, j, k)] = glm::vec3(x, y + 1.f, cell_unit * k);
            }
        }
    }

    // drawing as a cube of quads 
    for (size_t i = 1; i <= x_dim; i++) {
        for (size_t j = 1; j <= y_dim; j++) {
            for (size_t k = 1; k <= z_dim; k++) {
                indices.push_back(VERTEX(i+1, j+1, k+1));
                indices.push_back(VERTEX(i  , j+1, k+1));
                indices.push_back(VERTEX(i  , j  , k+1));

                indices.push_back(VERTEX(i+1, j+1, k+1));
                indices.push_back(VERTEX(i  , j  , k+1));
                indices.push_back(VERTEX(i+1, j  , k+1));

                //--
                
                indices.push_back(VERTEX(i+1, j+1, k  ));
                indices.push_back(VERTEX(i+1, j+1, k+1));
                indices.push_back(VERTEX(i+1, j  , k+1));

                indices.push_back(VERTEX(i+1, j+1, k  ));
                indices.push_back(VERTEX(i+1, j  , k+1));
                indices.push_back(VERTEX(i+1, j  , k  ));

                //--
                
                indices.push_back(VERTEX(i  , j+1, k  ));
                indices.push_back(VERTEX(i+1, j+1, k  ));
                indices.push_back(VERTEX(i+1, j  , k  ));
                
                indices.push_back(VERTEX(i  , j+1, k  ));
                indices.push_back(VERTEX(i+1, j  , k  ));
                indices.push_back(VERTEX(i  , j  , k  ));

                //-- 
                indices.push_back(VERTEX(i  , j+1, k+1));
                indices.push_back(VERTEX(i  , j+1, k  ));
                indices.push_back(VERTEX(i  , j  , k  ));

                indices.push_back(VERTEX(i  , j+1, k+1));
                indices.push_back(VERTEX(i  , j  , k  ));
                indices.push_back(VERTEX(i  , j  , k+1));
                
                //-- 
                
                indices.push_back(VERTEX(i+1, j  , k  ));
                indices.push_back(VERTEX(i  , j  , k  ));
                indices.push_back(VERTEX(i  , j  , k+1));

                indices.push_back(VERTEX(i+1, j  , k  ));
                indices.push_back(VERTEX(i  , j  , k+1));
                indices.push_back(VERTEX(i+1, j  , k+1));
                
                //-- 
                
                indices.push_back(VERTEX(i+1, j+1, k  ));
                indices.push_back(VERTEX(i  , j+1, k  ));
                indices.push_back(VERTEX(i  , j+1, k+1));

                indices.push_back(VERTEX(i+1, j+1, k  ));
                indices.push_back(VERTEX(i  , j+1, k+1));
                indices.push_back(VERTEX(i+1, j+1, k+1));
            }
        }
    }


    srand(time(0));

    init();
}

void Fluid::clear() {
	for (int i=0; i<_total_size ; i++) {
		x_vel[i] = x_vel_prev[i] = y_vel[i] = y_vel_prev[i] = z_vel[i] = z_vel_prev[i] = dens[i] = dens_prev[i] = 0.f;
	}

    addforce[0] = addforce[1] = addforce[2] = 0;
}

void Fluid::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/fluid.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    check_gl_error();

    shader.enable();
    check_gl_error();

    glGenVertexArrays(1, &vao); //Create a VAO
    glGenBuffers(2, vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, _total_size * sizeof(glm::vec3), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, _total_size * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  
    glBindVertexArray(0);

    init_static_uniforms();
    shader.disable();
    check_gl_error();
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

    check_gl_error();
    
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
    check_gl_error();
}

void Fluid::update_force_source(float * d, float * u, float * v, float * w ) {

    #pragma omp parallel for
    for (int i = 0; i < _total_size; i++) {
		d[i] = u[i] = v[i] = w[i] = 0.0f;
	}

    // for (size_t i = 1; i <= _x_dim; i++) {
    //     for (size_t j = 1; j <= _y_dim; j++) {
    //         for (size_t k = 1; k <= _z_dim; k++) {
    //             if (j > _y_dim-5) {
    //                 v[VERTEX(i,j,k)] = -1.f;
    //             }
    //         }
    //     }
    // }

    int i, j, k;
    if(addforce[0]!=0) // x
	{
		i=2,
		j=_y_dim/2;
		k=_z_dim/2;

		if ( i<1 || i>_x_dim || j<1 || j>_y_dim || k <1 || k>_z_dim) return;
		u[VERTEX(i,j,k)] = force*addforce[0];
		// u[VERTEX(i,j+1,k)] = force*addforce[0];
		// u[VERTEX(i,j,k+1)] = force*addforce[0];
		// u[VERTEX(i,j+1,k+1)] = force*addforce[0];
		addforce[0] = 0;
	}	

	// if(addforce[1]!=0)
	{
		i=_x_dim/2,
		j=2;
		k=_z_dim/2;

		if ( i<1 || i>_x_dim || j<1 || j>_y_dim || k <1 || k>_z_dim) return;
		v[VERTEX(i,j,k)] = -0.5f * force; //*addforce[1];
		// v[VERTEX(i+1,j,k)] = force*addforce[1];
		// v[VERTEX(i,j,k+1)] = force*addforce[1];
		// v[VERTEX(i+1,j,k+1)] = force*addforce[1];
		addforce[1] = 0;
	}	

    // v[VERTEX(i,j,k)] += 10.f;

	if(addforce[2]!=0) // y
	{
		i=_x_dim/2,
		j=_y_dim/2;
		k=2;

		if ( i<1 || i>_x_dim || j<1 || j>_y_dim || k <1 || k>_z_dim) return;
		w[VERTEX(i,j,k)] = force*addforce[2]; 	
		addforce[2] = 0;
	}	

	if(addsource==1)
	{
		i=_x_dim/2,
		j=_y_dim-1;
		k=_z_dim/2;
		d[VERTEX(i,j,k)] = source;
		addsource = 0;
	}
}

void Fluid::update(float dt) {
    update_force_source(dens_prev, x_vel_prev, y_vel_prev, z_vel_prev);
    vel_step(_x_dim, _y_dim, _z_dim, x_vel, y_vel, z_vel,  x_vel_prev, y_vel_prev, z_vel_prev, visc, dt );
	dens_step(_x_dim, _y_dim, _z_dim, dens, dens_prev, x_vel, y_vel, z_vel, diff, dt );
}

void Fluid::key_down(int key) {
    switch ( key ) {
        case GLFW_KEY_UP: addforce[0] = 1; break;
        case GLFW_KEY_DOWN: addforce[0] = -1; break;
        case GLFW_KEY_G: addforce[1] = -1; break;
        case GLFW_KEY_LEFT: addforce[2] = 1; break;
        case GLFW_KEY_RIGHT: addforce[2] = -1; break;
        case GLFW_KEY_E: addsource = 1; break;
    }

    // std::cout << key << " " << addforce[0] << " " << addforce[1] << " " << addforce[2] << std::endl;
}

void Fluid::draw() {
    check_gl_error();

    shader.enable();

    GLint attribVertexPosition  = shader.attribute("in_position");
    // GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");
	GLint attribVertexDensity    = shader.attribute("in_density");

	GLint attribUniformView    = shader.uniform("view");
	GLint attribUniformProjection    = shader.uniform("projection");
	GLint attribUniformModel    = shader.uniform("model");
	GLint attribUniformNormal    = shader.uniform("normal");

    glBindVertexArray(vao);

    // glVertexAttrib3f(attribVertexColor, 0.1f, 0.3f, 0.8f);
    glVertexAttrib3f(attribVertexNormal, 0.f, 1.f, 0.f);

   
    // glm::mat4 fluid_model = glm::translate(  // Scale first
    //     glm::mat4( 1.0f ),              // Translate second
    //     glm::vec3( 0.0f, 2.0f, 0.0f )
    // );
    glm::mat4 fluid_model = glm::mat4( 1.0f );

    glm::mat4 matrix_fluid_normal = fluid_model;
    matrix_fluid_normal[3] = glm::vec4(0,0,0,1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);    
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); 
    glEnableVertexAttribArray(attribVertexPosition);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, _total_size * sizeof(float), dens, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(attribVertexDensity);
    glVertexAttribPointer(attribVertexDensity, 1, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glUniformMatrix4fv( attribUniformView, 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( attribUniformProjection, 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
    glUniformMatrix4fv( attribUniformModel, 1, GL_FALSE, glm::value_ptr(fluid_model)  ); // model transformation
    glUniformMatrix4fv( attribUniformNormal, 1, GL_FALSE, glm::value_ptr(matrix_fluid_normal)); // projection matrix

    // draw the fluid
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glDrawElements(GL_TRIANGLES,                    // primitive type
                indices.size(),          // # of indices
                GL_UNSIGNED_SHORT,                 // data type
                (void*)0);                       // offset to indices


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
    glBindVertexArray(0);

    shader.disable();
    check_gl_error();

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
                closest = f;
            }
        }
    } 

    // if (mouse_down) {
    //     next_source = closest;
    // }
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

