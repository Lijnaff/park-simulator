#include "window.h"

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    
    // Request an OpenGL 3.2 Core Profile Context
    // This is required on macOS to use Shaders and VAOs properly!
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Park Simulator - Modern OpenGL");

    // Initialize OpenGL state and assets
    initOpenGL();

    // Register callbacks
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutMouseFunc(mouseButtonCallback);
    glutMotionFunc(activeMotionCallback);
    glutIdleFunc(idleCallback);

    // Enter main event loop
    glutMainLoop();

    return 0;
}
