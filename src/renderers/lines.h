#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include <vector>
#include <array>
#include <glm/mat4x4.hpp>

#include "../utils/shader.h"


class PathRenderer
{
    Shader shader;
    GLuint vao;
    GLuint vbo_milestones;
    GLuint vbo_edges;

    public:
        PathRenderer();

        void draw_edges(const std::vector<glm::vec3> edges, const glm::vec4 color);
        void draw_milestones(const std::vector<glm::vec3> milestones, const glm::vec4 color);
        void cleanup();
};
#endif // LINE_RENDERER_H