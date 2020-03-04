#include "../common.h"

#include <iostream>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
#include "text2d.h"

using namespace glm;

#include <ft2build.h>
#include FT_FREETYPE_H

GLuint texture_id;
GLuint vao; // Vertexes
GLuint vbo; // Vertexes

static FT_Library ft;
static FT_Face face;

std::map<GLchar, Character> Characters;

Shader text_shader;

void initText2D(const char * font){
    std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/text.";
    text_shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );
    text_shader.enable();

    GLuint uniformTextProjection = text_shader.uniform("projection");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(Globals::screen_width), 0.0f, static_cast<GLfloat>(Globals::screen_height));
    glUniformMatrix4fv(uniformTextProjection, 1, GL_FALSE, glm::value_ptr(projection));

    std::stringstream font_ss; font_ss << MY_MODELS_DIR << "fonts/" << font;
    std::cout << "Loading font" << font_ss.str() << std::endl;
    
    if (FT_Init_FreeType(&ft))
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    if (FT_New_Face(ft, font_ss.str().c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        exit(0);
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    GLuint attribVertex = text_shader.attribute("in_vertex");
    glEnableVertexAttribArray(attribVertex);
    glVertexAttribPointer(attribVertex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    text_shader.enable();

    GLuint uniformTextColor = text_shader.uniform("textColor");
    glUniform3f(uniformTextColor, color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// void printText2D(const char * text, float x, float y, float size){
//     check_gl_error();
// 	unsigned int length = strlen(text);

// 	// Fill buffers
// 	std::vector<glm::vec2> vertices;
// 	std::vector<glm::vec2> UVs;
// 	for ( unsigned int i=0 ; i<length ; i++ ){
		
// 		glm::vec2 vertex_up_left    = glm::vec2( x+i*size     , y+size );
// 		glm::vec2 vertex_up_right   = glm::vec2( x+i*size+size, y+size );
// 		glm::vec2 vertex_down_right = glm::vec2( x+i*size+size, y      );
// 		glm::vec2 vertex_down_left  = glm::vec2( x+i*size     , y      );

// 		vertices.push_back(vertex_up_left   );
// 		vertices.push_back(vertex_down_left );
// 		vertices.push_back(vertex_up_right  );

// 		vertices.push_back(vertex_down_right);
// 		vertices.push_back(vertex_up_right);
// 		vertices.push_back(vertex_down_left);

// 		char character = text[i];
// 		float uv_x = (character%16)/16.0f;
// 		float uv_y = (character/16)/16.0f;

// 		glm::vec2 uv_up_left    = glm::vec2( uv_x           , 1.0f - uv_y );
// 		glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/16.0f, 1.0f - uv_y );
// 		glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/16.0f, 1.0f - (uv_y + 1.0f/16.0f) );
// 		glm::vec2 uv_down_left  = glm::vec2( uv_x           , 1.0f - (uv_y + 1.0f/16.0f) );
// 		UVs.push_back(uv_up_left   );
// 		UVs.push_back(uv_down_left );
// 		UVs.push_back(uv_up_right  );

// 		UVs.push_back(uv_down_right);
// 		UVs.push_back(uv_up_right);
// 		UVs.push_back(uv_down_left);
// 	}
//     check_gl_error();

//     // Bind text_shader
// 	text_shader.enable();

//     glBindVertexArray(vao);

// 	glBindBuffer(GL_ARRAY_BUFFER, vbo);
// 	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
// 	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
// 	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
//     check_gl_error();
	
// 	// Bind texture
// 	glActiveTexture(GL_TEXTURE0);
// 	glBindTexture(GL_TEXTURE_2D, texture_id);
// 	// Set our "myTextureSampler" sampler to user Texture Unit 0
//     GLuint uniformTexture = text_shader.uniform("myTextureSampler");
//     GLuint attribVertexPosition = text_shader.attribute("in_position");
//     GLuint attribVertexUV = text_shader.attribute("in_uv");
// 	glUniform1i(uniformTexture, 0);

//     check_gl_error();

//     glEnableVertexAttribArray(attribVertexPosition);
//     glEnableVertexAttribArray(attribVertexUV);
//     check_gl_error();

// 	// 1rst attribute buffer : vertices
// 	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     check_gl_error();
// 	glVertexAttribPointer(attribVertexPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
//     check_gl_error();
	
// check_gl_error();
// 	// 2nd attribute buffer : UVs
// 	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
// 	glVertexAttribPointer(attribVertexUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	
// check_gl_error();
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// 	// Draw call
// 	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
// check_gl_error();
// 	glDisable(GL_BLEND);

// 	glDisableVertexAttribArray(0);
// 	glDisableVertexAttribArray(1);

//     glBindVertexArray(0);

// }

void cleanupText2D(){


	// Delete buffers
	glDeleteBuffers(1, &vbo);

	// Delete texture
	glDeleteTextures(1, &texture_id);

    glDeleteVertexArrays(1, &vao);
}