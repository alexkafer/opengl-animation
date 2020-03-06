#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include <vector>
#include <glm/mat4x4.hpp>

#include "../utils/shader.h"


class LineRenderer
{
    Shader shader;
    GLuint vao;
    GLuint vbo;

    public:
        LineRenderer();

        void draw(const std::vector<glm::vec3> milestones);
        void cleanup();
};
#endif // LINE_RENDERER_H