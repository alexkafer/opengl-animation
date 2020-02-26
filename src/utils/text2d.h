#ifndef TEXT2D_HPP
#define TEXT2D_HPP

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

void initText2D(const char * texturePath);
// void printText2D(const char * text, float x, float y, float size);
void cleanupText2D();
void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

#endif