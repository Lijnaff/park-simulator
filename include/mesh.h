#ifndef MESH_H
#define MESH_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include <vector>

struct Vertex {
    float Position[3];
    float Normal[3];
    float TexCoords[2];
};

class Mesh {
public:
    GLuint VAO, VBO;
    int vertexCount;

    Mesh();
    ~Mesh();
    void setupMesh(const std::vector<Vertex>& vertices);
    void draw() const;

    // Static generators
    static float getTerrainHeight(float x, float z);
    static Mesh* createTerrain(float size, int resolution);
    static Mesh* createPlane(float size, float texTiling);
    static Mesh* createCube(float width, float height, float depth);
};

#endif
