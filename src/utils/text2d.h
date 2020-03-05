#ifndef TEXT2D_HPP
#define TEXT2D_HPP

#include <map>

#include "../common.h"
#include "shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

class Text2D {
    GLuint texture_id;
    GLuint texture_vao; // Vertexes
    GLuint texture_vbo; // Vertexes

    FT_Library ft;
    FT_Face face;

    std::map<GLchar, Character> characters;

    Shader text_shader;

public:
    Text2D();

    void init(const char * texturePath); 
    void draw(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color); 
    void cleanup();
};

#endif