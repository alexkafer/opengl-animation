#include <iostream>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "texture.h"

// Created by examining https://github.com/syoyo/tinyobjloader/blob/master/examples/viewer/viewer.cc
static bool FileExists(const char * abs_filename) {
    bool ret;
    FILE* fp = fopen(abs_filename, "rb");
    if (fp) {
        ret = true;
        fclose(fp);
    } else {
        ret = false;
    }

    return ret;
}

GLuint load_texture(const char * texturePath) {
    GLuint texture_id;

    if (!FileExists(texturePath)) {
        // Append base dir.
        std::cerr << "Unable to find file: " << texturePath << std::endl;
        exit(1);
    }

    int w, h, comp;
    unsigned char* image = stbi_load(texturePath, &w, &h, &comp, STBI_default);
    if (!image) {
        std::cerr << "Unable to load texture: " << texturePath << std::endl;
        exit(1);
    }
    std::cout << "Loaded texture: " << texturePath << ", w = " << w
                << ", h = " << h << ", comp = " << comp << std::endl;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (comp == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                    GL_UNSIGNED_BYTE, image);
    } else if (comp == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, image);
    } else {
        assert(0);  // TODO
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);

    return texture_id;
}