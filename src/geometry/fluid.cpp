#include "fluid.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/normal.hpp>

#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
#define END_FOR }}

Fluid::Fluid(size_t x_dim, size_t z_dim) {
    int size = (N+2)*(N+2);

    _x_dim = x_dim;
    _y_dim = z_dim;
    // size = (dimension + 2) * (dimension + 2);
    points = std::vector<glm::vec3>();

    u = new float [size];
    v = new float [size];
    u_prev = new float[size];
    v_prev = new float[size];
    dens = new float[size];
    dens_prev = new float[size];

    indices = std::vector<GLushort>();

    size_t vertex[x_dim][z_dim];
    float h = 1.0f / x_dim - 0.5f;
    float w = 1.0f / z_dim - 0.5f;
    for (size_t i = 0; i < x_dim; i++) {
        float x = h * i;
        for (size_t j = 0; j < z_dim; j++) {
            float z = w * j;
            vertex[i][j] = points.size();

            points.push_back(glm::vec3(x, 0.5f, z));
            std::cout << "Placing at " << x << " and " << z << std::endl; 
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

void Fluid::clear() {
    int size = (N+2)*(N+2);

	for (int i=0; i<size ; i++) {
		u[i] = v[i] = u_prev[i] = v_prev[i] = dens[i] = dens_prev[i] = 0.0f;
	}
}

void Fluid::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/fluid.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    glGenVertexArrays(1, &vao); //Create a VAO

    std::cout << "Starting vbos" << std::endl;

    glGenBuffers(2, vbo);
    std::cout << "genned vbos" << std::endl;
    

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, dens->size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);


    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  
    glBindVertexArray(0);

    init_static_uniforms();
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
   vel_step(dt)
   dens_step(0.0f, dt)
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
    glBufferData(GL_ARRAY_BUFFER, dens->size() * sizeof(float), &(dens->data), GL_DYNAMIC_DRAW);
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

void set_bnd(float * x )
{
    int N = _x_dim;

	for (int i=1 ; i <= N; i++) {
		x[IX(0  ,i)] = x[IX(1,i)];
		x[IX(N+1,i)] = x[IX(N,i)];
		x[IX(i,0  )] = x[IX(i,1)];
		x[IX(i,N+1)] = x[IX(i,N)];
	}

	x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
	x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
	x[IX(N+1,0  )] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
	x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);
}

void set_vel_bnd(float * x )
{
    int N = _x_dim;

	for (int i=1; i <= N; i++) {
        x[IX(0  ,i)] = x[IX(1,i)];
        x[IX(N+1,i)] = x[IX(N,i)];

        x[IX(i,0  )] = x[IX(i,1)];
        x[IX(i,N+1)] = x[IX(i,N)];

        // X
        x[IX(0  ,i)].x *= -1;
        x[IX(N+1,i)].x *= -1;

        // Y
        x[IX(i,0  )].y *= -1;
        x[IX(i,N+1)].y *= -1;
	}

	x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
	x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)]+x[IX(0  ,N)]);
	x[IX(N+1,0  )] = 0.5f*(x[IX(N,0  )]+x[IX(N+1,1)]);
	x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)]+x[IX(N+1,N)]);
}

void lin_solve(float * x, float * x0, float a, float c)
{
	for (int k=0 ; k<20 ; k++ ) {
		for (int i=1 ; i <= _x_dim ; i++ ) { 
            for (int j=1 ; j <= _z_dim ; j++ ) {
			    x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
		    }
        }

		set_bnd (x);
	}
}

void lin_solve(glm::vec2  * x, glm::vec2 * x0, float a, float c)
{
	for (int k=0 ; k<20 ; k++ ) {
		for (int i=1 ; i <= _x_dim ; i++ ) { 
            for (int j=1 ; j <= _z_dim ; j++ ) {
			    x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
		    }
        }

		set_vel_bnd(x);
	}
}

void diffuse(float * x, float * x0, float diff, float dt )
{
	float a = dt * diff * _x_dim * _z_dim;
	lin_solve(x, x0, a, 1+4*a);
}

void diffuse(glm::vec2 * x, glm::vec2 * x0, float diff, float dt )
{
	float a = dt * diff * _x_dim * _z_dim;
	lin_solve(x, x0, a, 1+4*a);
}

void advect(int b, float * d, float * d0, float * u, float * v, float dt)
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1;

	float dt0 = dt*N;
    for (int i=1 ; i <= _x_dim ; i++ ) { 
        for (int j=1 ; j <= _z_dim ; j++ ) {
            x = i-dt0*u[IX(i,j)]; 
            y = j-dt0*v[IX(i,j)];
            
            if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
            if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;

            s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
            d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
                        s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
        }
    }

	set_bnd(b, d);
}

void project(glm::vec2 vel, float * p, float * div)
{
    for (int i=1; i <= _x_dim; i++) { 
        for (int j=1; j <= _z_dim ; j++) {
            div[IX(i,j)] = -0.5f*(vel[IX(i+1,j)].x-vel[IX(i-1,j)].x+vel[IX(i,j+1)].y-vel[IX(i,j-1)].y)/_x_dim;
            p[IX(i,j)] = 0;
        }
    }	

	set_bnd(div); set_bnd(p);

	lin_solve (0, p, div, 1, 4 );

    for (int i=1 ; i<=_x_dim ; i++ ) { 
        for (int j=1 ; j<=_z_dim ; j++ ) {
            vel[IX(i,j)].x -= 0.5f*_x_dim*(p[IX(i+1,j)]-p[IX(i-1,j)]);
            vel[IX(i,j)].y -= 0.5f*_x_dim*(p[IX(i,j+1)]-p[IX(i,j-1)]);
        }
    }

    set_vel_bnd(vel);
}


// dens_step ( N, dens, dens_prev, u, v, diff, dt );
// void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
// void Fluid::dens_step(int N, float * x, float * x0, float * u, float * v, float diff, float dt )
void Fluid::dens_step(float dt)
{
    // Update previous
    int size=(_x_dim+2)*(_z_dim+2);
	for (int i=0; i < size; i++) {
        x[i] += dt * x0[i];
    }

	SWAP ( x0, x ); diffuse(x, x0, diff, dt);
	SWAP ( x0, x ); advect(x, x0, u, v, dt);
}

// vel_step ( N, u, v, u_prev, v_prev, visc, dt );
// void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt )
void Fluid::vel_step (float dt )
{
    // Update previous
    int size=(_x_dim+2)*(_z_dim+2);
	for (int i=0; i < size; i++) {
        velocity[i] += dt * velocity_prev[i];
    }

    SWAP ( velocity_prev, velocity ); diffuse(velocity, velocity_prev, visc, dt);

	project(velocity, velocity_prev);
	SWAP ( velocity_prev, velocity );
	advect ( N, 1, u, u0, u0, v0, dt );
    advect ( N, 2, v, v0, u0, v0, dt );
	project(velocity, velocity_prev);
}




void Fluid::cleanup() {
    glDeleteVertexArrays(1, &vao);

    delete [] u;
    delete [] v_prev;
    delete [] u_prev;
    delete [] v_prev;
    delete [] dens;
    delete [] dens_prev;
}