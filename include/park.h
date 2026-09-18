#ifndef PARK_H
#define PARK_H

#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include "mesh.h"

// Structure to hold 2D positions for objects like trees
struct Position2D {
    float x;
    float z;
};

class Park {
public:
    Park();
    ~Park();
    
    // Initialize OpenGL assets (textures, shaders, meshes)
    void init();

    // Main render function to draw the entire park
    void draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos, float yaw);

private:
    std::vector<glm::vec3> trees;
    GLuint texGrass;
    GLuint texBark;
    GLuint texLeaves;
    GLuint texAsphalt;
    GLuint texSign;
    GLuint texWater;
    GLuint texRestaurant;
    GLuint texShop;
    
    Shader* standardShader;
    Shader* uiShader;
    
    Mesh* groundMesh;
    Mesh* trunkMesh;
    Mesh* leafMesh;
    Mesh* boxMesh;
    Mesh* waterMesh;
    Mesh* uiMesh;
    Mesh* arrowMesh;

    void generateTrees(int count, float areaSize);
    
    // Path rendering helpers
    void drawPathStraight(glm::vec3 start, glm::vec3 end, float width);
    void drawPathCurve(glm::vec3 center, float radius, float startAngle, float endAngle, float width);
    
    // Props
    void drawFence(float size);
    void drawBuilding(glm::vec3 position);
    void drawSign(glm::vec3 position, float rotationY);
    void drawRestaurant(glm::vec3 position, float rotationY);
    void drawShop(glm::vec3 position, float rotationY);
    void drawHUD(const glm::vec3& viewPos, float yaw);
};

#endif // PARK_H
