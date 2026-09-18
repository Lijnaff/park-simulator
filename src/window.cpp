#include "window.h"
#include "camera.h"
#include "park.h"
#include <chrono>

int windowWidth = 800;
int windowHeight = 600;

Camera camera;
Park park;

bool keys[256] = {false};
bool isDragging = false;
bool isSprinting = false;
int lastX = 400;
int lastY = 300;
auto lastFrameTime = std::chrono::high_resolution_clock::now();

void initOpenGL() {
    // Fog color matching sky
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Core profile no longer has GL_LIGHTING or GL_FOG built-in!
    // We implement these entirely in our custom shaders now.
    park.init();
}

void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute matrices (Increased far plane to 300.0f for deeper LOS)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 300.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // Draw the modern park
    park.draw(view, projection, camera.Position, camera.Yaw);

    glutSwapBuffers();
}

void reshapeCallback(int width, int height) {
    if (height == 0) height = 1;
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void keyboardCallback(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) exit(0); // ESC to quit
    if (key == 'r' || key == 'R') camera.teleportToGate();
    
    keys[key] = true;
    
    // Check for SHIFT key modifier
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) isSprinting = true;
    else isSprinting = false;
}

void keyboardUpCallback(unsigned char key, int x, int y) {
    (void)x; (void)y;
    keys[key] = false;
    
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) isSprinting = true;
    else isSprinting = false;
}

void mouseButtonCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastX = x;
            lastY = y;
        } else if (state == GLUT_UP) {
            isDragging = false;
        }
    }
}

void activeMotionCallback(int x, int y) {
    if (isDragging) {
        float xoffset = x - lastX;
        float yoffset = y - lastY; // Reversed since y-coordinates go from bottom to top
        lastX = x;
        lastY = y;
        camera.processMouseMovement(xoffset, yoffset);
    }
}

void idleCallback() {
    // Calculate delta time
    auto currentFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = currentFrameTime - lastFrameTime;
    float deltaTime = duration.count();
    lastFrameTime = currentFrameTime;

    // Apply Sprinting speed
    if (isSprinting || keys[' ']) { // Support SHIFT or SPACEBAR to sprint
        camera.MovementSpeed = 20.0f;
    } else {
        camera.MovementSpeed = 5.0f;
    }

    // Process continuous movement
    camera.processKeyboard(keys['w'], keys['s'], keys['a'], keys['d'], deltaTime);

    // Request new frame
    glutPostRedisplay();
}
