#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

// Loads an image file and returns an OpenGL texture ID
GLuint loadTexture(const char* filepath);

#endif // TEXTURE_H
