#ifndef WINDOW_H
#define WINDOW_H

// Core OpenGL and GLUT headers for macOS
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>
#endif

// Global constants
extern int windowWidth;
extern int windowHeight;

// Function prototypes
void initOpenGL();
void displayCallback();
void reshapeCallback(int width, int height);

// Input callbacks
void keyboardCallback(unsigned char key, int x, int y);
void keyboardUpCallback(unsigned char key, int x, int y);
void mouseButtonCallback(int button, int state, int x, int y);
void activeMotionCallback(int x, int y);
void idleCallback();

#endif // WINDOW_H
