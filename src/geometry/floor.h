#ifndef FLOOR_H
#define FLOOR_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include "../common.h"
#include "../utils/shader.h"
#include "../renderers/renderable.h"

class Floor: public Renderable {
    GLuint vao;
    GLuint vbo[2];

public:
    Floor();

    void init(Shader & shader);
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();
};

#endif // FLOOR_H

