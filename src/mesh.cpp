#include "mesh.h"
#include <glm/glm.hpp>
#include <cmath>

Mesh::Mesh() : VAO(0), VBO(0), vertexCount(0) {}

Mesh::~Mesh() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void Mesh::setupMesh(const std::vector<Vertex>& vertices) {
    vertexCount = vertices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

float Mesh::getTerrainHeight(float x, float z) {
    float dist = glm::length(glm::vec2(x, z));
    // Smooth, wide rolling hills instead of spiky noise
    float baseHeight = sin(x * 0.015f) * cos(z * 0.015f) * 8.0f;
    baseHeight += sin(x * 0.005f) * sin(z * 0.008f) * 6.0f;
    baseHeight = abs(baseHeight);
    
    float attenuation = 1.0f;
    if (abs(x) < 20.0f) {
        float factor = (abs(x) - 8.0f) / 12.0f;
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }
    if (abs(z) < 20.0f) {
        float factor = (abs(z) - 8.0f) / 12.0f; 
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }
    if (dist < 70.0f) {
        float factor = (dist - 50.0f) / 20.0f;
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }
    if (dist > 220.0f && dist < 280.0f) {
        float ringDist = abs(dist - 250.0f);
        if (ringDist < 20.0f) {
            float factor = (ringDist - 8.0f) / 12.0f;
            if (factor < 0.0f) factor = 0.0f;
            attenuation *= factor;
        }
    }
    
    // Attenuate near branch path to the sign (x from 0 to 90, z around -80)
    if (x > 0.0f && x < 90.0f && abs(z + 80.0f) < 15.0f) {
        float factor = (abs(z + 80.0f) - 5.0f) / 10.0f; 
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }
    
    // Attenuate around the Sign Plaza itself
    float distToSign = glm::length(glm::vec2(x - 80.0f, z + 80.0f));
    if (distToSign < 25.0f) {
        float factor = (distToSign - 12.0f) / 13.0f;
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }
    
    // Carve a deep basin for the Pond at (-100, -100)
    float distToPond = glm::length(glm::vec2(x + 100.0f, z + 100.0f));
    if (distToPond < 40.0f) {
        float factor = (distToPond - 20.0f) / 20.0f;
        if (factor < 0.0f) factor = 0.0f;
        
        float currentH = baseHeight * attenuation * factor;
        
        // Deep crater inside the pond
        if (distToPond < 20.0f) {
            float depthFactor = distToPond / 20.0f; // 0 at center, 1 at edge
            currentH = -4.0f * (1.0f - depthFactor); // Drops down to -4.0m
        }
        
        return currentH;
    }
    
    // Attenuate for East Restaurant (250, 150)
    float distToEastRest = glm::length(glm::vec2(x - 250.0f, z - 150.0f));
    if (distToEastRest < 30.0f) {
        float factor = (distToEastRest - 20.0f) / 10.0f;
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }
    
    // Attenuate for West Restaurant (-100, -30)
    float distToWestRest = glm::length(glm::vec2(x + 100.0f, z + 30.0f));
    if (distToWestRest < 30.0f) {
        float factor = (distToWestRest - 20.0f) / 10.0f;
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }

    // Attenuate for Shop (50, 50)
    float distToShop = glm::length(glm::vec2(x - 50.0f, z - 50.0f));
    if (distToShop < 20.0f) {
        float factor = (distToShop - 12.0f) / 8.0f;
        if (factor < 0.0f) factor = 0.0f;
        attenuation *= factor;
    }

    return baseHeight * attenuation;
}

Mesh* Mesh::createTerrain(float size, int resolution) {
    std::vector<Vertex> vertices;
    float step = size / resolution;
    float start = -size / 2.0f;
    
    auto getNormal = [&](float x, float z) {
        float hL = getTerrainHeight(x - 0.1f, z);
        float hR = getTerrainHeight(x + 0.1f, z);
        float hD = getTerrainHeight(x, z - 0.1f);
        float hU = getTerrainHeight(x, z + 0.1f);
        glm::vec3 normal(hL - hR, 0.2f, hD - hU);
        return glm::normalize(normal);
    };

    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            float x0 = start + j * step;
            float z0 = start + i * step;
            float x1 = x0 + step;
            float z1 = z0 + step;
            
            float y00 = getTerrainHeight(x0, z0);
            float y10 = getTerrainHeight(x1, z0);
            float y01 = getTerrainHeight(x0, z1);
            float y11 = getTerrainHeight(x1, z1);
            
            glm::vec3 n00 = getNormal(x0, z0);
            glm::vec3 n10 = getNormal(x1, z0);
            glm::vec3 n01 = getNormal(x0, z1);
            glm::vec3 n11 = getNormal(x1, z1);
            
            vertices.push_back({{x0, y00, z0}, {n00.x, n00.y, n00.z}, {(x0-start)/4.0f, (z0-start)/4.0f}});
            vertices.push_back({{x0, y01, z1}, {n01.x, n01.y, n01.z}, {(x0-start)/4.0f, (z1-start)/4.0f}});
            vertices.push_back({{x1, y11, z1}, {n11.x, n11.y, n11.z}, {(x1-start)/4.0f, (z1-start)/4.0f}});
            
            vertices.push_back({{x1, y11, z1}, {n11.x, n11.y, n11.z}, {(x1-start)/4.0f, (z1-start)/4.0f}});
            vertices.push_back({{x1, y10, z0}, {n10.x, n10.y, n10.z}, {(x1-start)/4.0f, (z0-start)/4.0f}});
            vertices.push_back({{x0, y00, z0}, {n00.x, n00.y, n00.z}, {(x0-start)/4.0f, (z0-start)/4.0f}});
        }
    }
    
    Mesh* mesh = new Mesh();
    mesh->setupMesh(vertices);
    return mesh;
}

Mesh* Mesh::createPlane(float size, float texTiling) {
    std::vector<Vertex> vertices = {
        // positions            // normals           // texcoords
        {{-size, 0.0f, -size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-size, 0.0f,  size}, {0.0f, 1.0f, 0.0f}, {0.0f, texTiling}},
        {{ size, 0.0f,  size}, {0.0f, 1.0f, 0.0f}, {texTiling, texTiling}},

        {{ size, 0.0f,  size}, {0.0f, 1.0f, 0.0f}, {texTiling, texTiling}},
        {{ size, 0.0f, -size}, {0.0f, 1.0f, 0.0f}, {texTiling, 0.0f}},
        {{-size, 0.0f, -size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}
    };
    Mesh* mesh = new Mesh();
    mesh->setupMesh(vertices);
    return mesh;
}
Mesh* Mesh::createCube(float w, float h, float d) {
    w/=2; h/=2; d/=2;
    std::vector<Vertex> vertices = {
        // Back face
        {{-w, -h, -d},  {0.0f,  0.0f, -1.0f},  {0.0f, 0.0f}}, // Bottom-left
        {{ w,  h, -d},  {0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}}, // top-right
        {{ w, -h, -d},  {0.0f,  0.0f, -1.0f},  {1.0f, 0.0f}}, // bottom-right         
        {{ w,  h, -d},  {0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}}, // top-right
        {{-w, -h, -d},  {0.0f,  0.0f, -1.0f},  {0.0f, 0.0f}}, // bottom-left
        {{-w,  h, -d},  {0.0f,  0.0f, -1.0f},  {0.0f, 1.0f}}, // top-left
        // Front face
        {{-w, -h,  d},  {0.0f,  0.0f,  1.0f},  {0.0f, 0.0f}}, // bottom-left
        {{ w, -h,  d},  {0.0f,  0.0f,  1.0f},  {1.0f, 0.0f}}, // bottom-right
        {{ w,  h,  d},  {0.0f,  0.0f,  1.0f},  {1.0f, 1.0f}}, // top-right
        {{ w,  h,  d},  {0.0f,  0.0f,  1.0f},  {1.0f, 1.0f}}, // top-right
        {{-w,  h,  d},  {0.0f,  0.0f,  1.0f},  {0.0f, 1.0f}}, // top-left
        {{-w, -h,  d},  {0.0f,  0.0f,  1.0f},  {0.0f, 0.0f}}, // bottom-left
        // Left face
        {{-w,  h,  d},  {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}}, // top-right
        {{-w,  h, -d},  {-1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}}, // top-left
        {{-w, -h, -d},  {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}}, // bottom-left
        {{-w, -h, -d},  {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}}, // bottom-left
        {{-w, -h,  d},  {-1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}}, // bottom-right
        {{-w,  h,  d},  {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}}, // top-right
        // Right face
        {{ w,  h,  d},  {1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}}, // top-left
        {{ w, -h, -d},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}}, // bottom-right
        {{ w,  h, -d},  {1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}}, // top-right         
        {{ w, -h, -d},  {1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}}, // bottom-right
        {{ w,  h,  d},  {1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}}, // top-left
        {{ w, -h,  d},  {1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}}, // bottom-left     
        // Bottom face
        {{-w, -h, -d},  {0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}}, // top-right
        {{ w, -h, -d},  {0.0f, -1.0f,  0.0f},  {1.0f, 1.0f}}, // top-left
        {{ w, -h,  d},  {0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}}, // bottom-left
        {{ w, -h,  d},  {0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}}, // bottom-left
        {{-w, -h,  d},  {0.0f, -1.0f,  0.0f},  {0.0f, 0.0f}}, // bottom-right
        {{-w, -h, -d},  {0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}}, // top-right
        // Top face
        {{-w,  h, -d},  {0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}}, // top-left
        {{ w,  h,  d},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}}, // bottom-right
        {{ w,  h, -d},  {0.0f,  1.0f,  0.0f},  {1.0f, 1.0f}}, // top-right     
        {{ w,  h,  d},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}}, // bottom-right
        {{-w,  h, -d},  {0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}}, // top-left
        {{-w,  h,  d},  {0.0f,  1.0f,  0.0f},  {0.0f, 0.0f}}  // bottom-left        
    };
    Mesh* mesh = new Mesh();
    mesh->setupMesh(vertices);
    return mesh;
}
