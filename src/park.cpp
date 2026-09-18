#include "park.h"
#include "texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>

Park::Park() {
    texGrass = 0; texBark = 0; texLeaves = 0; texAsphalt = 0;
    standardShader = nullptr;
    groundMesh = nullptr;
    trunkMesh = nullptr;
    leafMesh = nullptr;
    boxMesh = nullptr;
    
    // Generate massive amount of trees over a huge area
    generateTrees(1500, 750.0f);
}

Park::~Park() {
    delete standardShader;
    delete groundMesh;
    delete trunkMesh;
    delete leafMesh;
    delete boxMesh;
}

void Park::init() {
    texGrass = loadTexture("assets/grass.jpg");
    texBark = loadTexture("assets/bark.jpg");
    texLeaves = loadTexture("assets/leaves.jpg");
    texAsphalt = loadTexture("assets/asphalt.jpg");
    texSign = loadTexture("assets/sign.jpg");
    texWater = loadTexture("assets/water.jpg");
    texRestaurant = loadTexture("assets/restaurant.jpg");
    texShop = loadTexture("assets/shop.jpg");

    standardShader = new Shader("assets/shaders/standard.vert", "assets/shaders/standard.frag");
    uiShader = new Shader("assets/shaders/ui.vert", "assets/shaders/ui.frag");

    // 800x800 world size Terrain
    groundMesh = Mesh::createTerrain(800.0f, 200); 
    trunkMesh = Mesh::createCube(0.6f, 2.0f, 0.6f);
    leafMesh = Mesh::createCube(2.4f, 2.4f, 2.4f);
    boxMesh = Mesh::createCube(1.0f, 1.0f, 1.0f);
    waterMesh = Mesh::createPlane(15.0f, 6.0f); // 30x30m plane, tile 6x

    // Create UI Quad (from 0 to 1)
    std::vector<Vertex> uiVerts = {
        {{0.0f, 0.0f, 0.0f}, {0,0,1}, {0,0}},
        {{0.0f, 1.0f, 0.0f}, {0,0,1}, {0,1}},
        {{1.0f, 1.0f, 0.0f}, {0,0,1}, {1,1}},
        {{1.0f, 1.0f, 0.0f}, {0,0,1}, {1,1}},
        {{1.0f, 0.0f, 0.0f}, {0,0,1}, {1,0}},
        {{0.0f, 0.0f, 0.0f}, {0,0,1}, {0,0}}
    };
    uiMesh = new Mesh();
    uiMesh->setupMesh(uiVerts);

    // Create Arrow Triangle (pointing UP)
    std::vector<Vertex> arrowVerts = {
        {{0.0f, -0.5f, 0.0f}, {0,0,1}, {0.5f, 0.0f}}, 
        {{-0.5f, 0.5f, 0.0f}, {0,0,1}, {0.0f, 1.0f}}, 
        {{0.5f, 0.5f, 0.0f}, {0,0,1}, {1.0f, 1.0f}}   
    };
    arrowMesh = new Mesh();
    arrowMesh->setupMesh(arrowVerts);
}

void Park::generateTrees(int count, float areaSize) {
    int spawned = 0;
    while (spawned < count) {
        glm::vec3 pos;
        pos.x = ((float)rand() / (float)RAND_MAX) * areaSize - (areaSize / 2.0f);
        pos.z = ((float)rand() / (float)RAND_MAX) * areaSize - (areaSize / 2.0f);
        
        bool onPath = false;
        
        // Massive Path Check (width = 10m, so radius = 5m + 3m clearance = 8m)
        if (abs(pos.x) < 8.0f) onPath = true; // Vertical Axis
        if (abs(pos.z) < 8.0f) onPath = true; // Horizontal Axis
        
        float dist = glm::length(glm::vec2(pos.x, pos.z));
        
        // Center Plaza (radius 40m + half-width 6m + clearance 3m = 49m)
        if (dist < 50.0f && dist > 30.0f) onPath = true; 
        
        // Outer Ring (radius 250m)
        if (dist > 242.0f && dist < 258.0f) onPath = true;
        
        // Branch Path to Sign
        if (pos.x > 0.0f && pos.x < 90.0f && abs(pos.z + 80.0f) < 8.0f) onPath = true;
        // Sign Plaza itself
        if (glm::length(glm::vec2(pos.x - 80.0f, pos.z + 80.0f)) < 20.0f) onPath = true;
        
        // Pond Area
        if (glm::length(glm::vec2(pos.x + 100.0f, pos.z + 100.0f)) < 25.0f) onPath = true;

        // Restaurants
        if (glm::length(glm::vec2(pos.x - 250.0f, pos.z - 150.0f)) < 30.0f) onPath = true;
        if (glm::length(glm::vec2(pos.x + 100.0f, pos.z + 30.0f)) < 25.0f) onPath = true;
        
        // Shop
        if (glm::length(glm::vec2(pos.x - 50.0f, pos.z - 50.0f)) < 15.0f) onPath = true;

        // Skip spawning if on the path
        if (!onPath) {
            pos.y = Mesh::getTerrainHeight(pos.x, pos.z);
            trees.push_back(pos);
            spawned++;
        }
    }
}

void Park::drawPathStraight(glm::vec3 start, glm::vec3 end, float width) {
    glm::vec3 dir = end - start;
    float length = glm::length(dir);
    if (length < 0.001f) return;
    
    float angle = atan2(dir.x, dir.z);
    glm::vec3 mid = (start + end) / 2.0f;
    
    // Base Asphalt
    standardShader->setBool("useTexture", true);
    standardShader->setVec2("texScale", glm::vec2(1.0f, length / 4.0f));
    glBindTexture(GL_TEXTURE_2D, texAsphalt);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mid.x, 0.02f, mid.z));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(width, 0.05f, length));
    standardShader->setMat4("model", model);
    boxMesh->draw();

    standardShader->setVec2("texScale", glm::vec2(1.0f, 1.0f));
    standardShader->setBool("useTexture", false);
    
    // White Edges
    standardShader->setVec3("solidColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
    glm::mat4 edgeL = glm::mat4(1.0f);
    edgeL = glm::translate(edgeL, glm::vec3(mid.x, 0.05f, mid.z));
    edgeL = glm::rotate(edgeL, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    edgeL = glm::translate(edgeL, glm::vec3(-width/2.0f + 0.2f, 0.0f, 0.0f));
    edgeL = glm::scale(edgeL, glm::vec3(0.2f, 0.05f, length));
    standardShader->setMat4("model", edgeL);
    boxMesh->draw();

    glm::mat4 edgeR = glm::mat4(1.0f);
    edgeR = glm::translate(edgeR, glm::vec3(mid.x, 0.05f, mid.z));
    edgeR = glm::rotate(edgeR, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    edgeR = glm::translate(edgeR, glm::vec3(width/2.0f - 0.2f, 0.0f, 0.0f));
    edgeR = glm::scale(edgeR, glm::vec3(0.2f, 0.05f, length));
    standardShader->setMat4("model", edgeR);
    boxMesh->draw();

    // Yellow Dashed Center Line
    standardShader->setVec3("solidColor", glm::vec3(1.0f, 0.8f, 0.0f));
    int numDashes = length / 4.0f;
    for(int i = 0; i < numDashes; ++i) {
        float offsetZ = -length/2.0f + (i * 4.0f) + 2.0f;
        glm::mat4 dashModel = glm::mat4(1.0f);
        dashModel = glm::translate(dashModel, glm::vec3(mid.x, 0.05f, mid.z));
        dashModel = glm::rotate(dashModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        dashModel = glm::translate(dashModel, glm::vec3(0.0f, 0.0f, offsetZ));
        dashModel = glm::scale(dashModel, glm::vec3(0.15f, 0.05f, 2.0f));
        standardShader->setMat4("model", dashModel);
        boxMesh->draw();
    }
}

void Park::drawPathCurve(glm::vec3 center, float radius, float startAngle, float endAngle, float width) {
    float arcLength = abs(endAngle - startAngle) * radius;
    int segments = fmax(4, (int)(arcLength / 4.0f)); 
    float angleStep = (endAngle - startAngle) / segments;
    
    for(int i = 0; i < segments; ++i) {
        float a1 = startAngle + i * angleStep;
        float a2 = startAngle + (i+1) * angleStep;
        
        glm::vec3 p1 = center + glm::vec3(sin(a1) * radius, 0.0f, cos(a1) * radius);
        glm::vec3 p2 = center + glm::vec3(sin(a2) * radius, 0.0f, cos(a2) * radius);
        
        drawPathStraight(p1, p2, width);
    }
}

void Park::drawBuilding(glm::vec3 pos) {
    standardShader->setBool("useTexture", false);
    glm::mat4 m;
    
    // Main walls (Beige)
    standardShader->setVec3("solidColor", glm::vec3(0.85f, 0.8f, 0.7f));
    m = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0.0f, 2.0f, 0.0f));
    m = glm::scale(m, glm::vec3(8.0f, 4.0f, 6.0f));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    // Roof (Dark Grey)
    standardShader->setVec3("solidColor", glm::vec3(0.2f, 0.2f, 0.2f));
    m = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0.0f, 4.25f, 0.0f));
    m = glm::scale(m, glm::vec3(9.0f, 0.5f, 7.0f));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    // Window (Glass Blue)
    standardShader->setVec3("solidColor", glm::vec3(0.3f, 0.6f, 0.9f));
    float faceX = (pos.x > 0) ? -4.01f : 4.01f; 
    m = glm::translate(glm::mat4(1.0f), pos + glm::vec3(faceX, 2.0f, 0.0f));
    m = glm::scale(m, glm::vec3(0.1f, 1.5f, 3.0f));
    standardShader->setMat4("model", m); boxMesh->draw();
}

void Park::drawSign(glm::vec3 pos, float rotationY) {
    standardShader->setBool("useTexture", false);
    
    auto drawBox = [&](float dx, float dy, float sx, float sy, glm::vec3 color) {
        standardShader->setVec3("solidColor", color);
        glm::mat4 m = glm::translate(glm::mat4(1.0f), pos);
        m = glm::rotate(m, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::translate(m, glm::vec3(dx - 18.0f, dy, 0.0f)); // Center the 36m wide sign
        m = glm::scale(m, glm::vec3(sx, sy, 1.0f));
        standardShader->setMat4("model", m);
        boxMesh->draw();
    };

    glm::vec3 blk = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 red = glm::vec3(0.9f, 0.15f, 0.15f);

    float px = 0.0f; 
    
    // I
    drawBox(px, 5.0f, 3.0f, 1.0f, blk);
    drawBox(px, 3.0f, 1.0f, 3.0f, blk);
    drawBox(px, 1.0f, 3.0f, 1.0f, blk);
    px += 4.0f;

    // Heart (Red)
    drawBox(px - 1.0f, 4.0f, 1.5f, 1.5f, red);
    drawBox(px + 1.0f, 4.0f, 1.5f, 1.5f, red);
    drawBox(px, 2.5f, 3.5f, 2.5f, red);
    drawBox(px, 1.0f, 1.5f, 1.5f, red);
    px += 5.0f;

    // E
    drawBox(px - 1.0f, 3.0f, 1.0f, 5.0f, blk);
    drawBox(px, 5.0f, 2.0f, 1.0f, blk);
    drawBox(px, 3.0f, 2.0f, 1.0f, blk);
    drawBox(px, 1.0f, 2.0f, 1.0f, blk);
    px += 4.0f;

    // T
    drawBox(px, 5.0f, 3.0f, 1.0f, blk);
    drawBox(px, 2.5f, 1.0f, 4.0f, blk);
    px += 4.0f;

    // H
    drawBox(px - 1.0f, 3.0f, 1.0f, 5.0f, blk);
    drawBox(px + 1.0f, 3.0f, 1.0f, 5.0f, blk);
    drawBox(px, 3.0f, 1.0f, 1.0f, blk);
    px += 4.0f;

    // I
    drawBox(px, 5.0f, 3.0f, 1.0f, blk);
    drawBox(px, 3.0f, 1.0f, 3.0f, blk);
    drawBox(px, 1.0f, 3.0f, 1.0f, blk);
    px += 4.0f;

    // O
    drawBox(px - 1.0f, 3.0f, 1.0f, 5.0f, blk);
    drawBox(px + 1.0f, 3.0f, 1.0f, 5.0f, blk);
    drawBox(px, 5.0f, 1.0f, 1.0f, blk);
    drawBox(px, 1.0f, 1.0f, 1.0f, blk);
    px += 4.0f;

    // P
    drawBox(px - 1.0f, 3.0f, 1.0f, 5.0f, blk);
    drawBox(px + 0.5f, 5.0f, 2.0f, 1.0f, blk);
    drawBox(px + 0.5f, 3.0f, 2.0f, 1.0f, blk);
    drawBox(px + 1.0f, 4.0f, 1.0f, 1.0f, blk);
    px += 4.0f;

    // I
    drawBox(px, 5.0f, 3.0f, 1.0f, blk);
    drawBox(px, 3.0f, 1.0f, 3.0f, blk);
    drawBox(px, 1.0f, 3.0f, 1.0f, blk);
    px += 4.0f;

    // A
    drawBox(px - 1.0f, 2.5f, 1.0f, 4.0f, blk);
    drawBox(px + 1.0f, 2.5f, 1.0f, 4.0f, blk);
    drawBox(px, 5.0f, 3.0f, 1.0f, blk);
    drawBox(px, 3.0f, 1.0f, 1.0f, blk);
}

void Park::drawFence(float size) {
    standardShader->setBool("useTexture", false);
    standardShader->setVec3("solidColor", glm::vec3(0.3f, 0.3f, 0.35f));
    
    float poleSpacing = 4.0f;
    float height = 2.5f;
    glm::mat4 m;
    
    for (float z = -size; z <= size; z += poleSpacing) {
        m = glm::translate(glm::mat4(1.0f), glm::vec3(-size, height/2.0f, z));
        m = glm::scale(m, glm::vec3(0.15f, height, 0.15f));
        standardShader->setMat4("model", m); boxMesh->draw();
        
        m = glm::translate(glm::mat4(1.0f), glm::vec3(size, height/2.0f, z));
        m = glm::scale(m, glm::vec3(0.15f, height, 0.15f));
        standardShader->setMat4("model", m); boxMesh->draw();
    }

    for (float x = -size; x <= size; x += poleSpacing) {
        if (abs(x) > 10.0f) { // Leave a 20m gap in the center for entrances
            m = glm::translate(glm::mat4(1.0f), glm::vec3(x, height/2.0f, -size));
            m = glm::scale(m, glm::vec3(0.15f, height, 0.15f));
            standardShader->setMat4("model", m); boxMesh->draw();
            
            m = glm::translate(glm::mat4(1.0f), glm::vec3(x, height/2.0f, size));
            m = glm::scale(m, glm::vec3(0.15f, height, 0.15f));
            standardShader->setMat4("model", m); boxMesh->draw();
        }
    }
    
    float railThickness = 0.1f;
    m = glm::translate(glm::mat4(1.0f), glm::vec3(-size, height - 0.2f, 0.0f));
    m = glm::scale(m, glm::vec3(railThickness, railThickness, size * 2.0f));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    m = glm::translate(glm::mat4(1.0f), glm::vec3(-size, height / 2.0f, 0.0f));
    m = glm::scale(m, glm::vec3(railThickness, railThickness, size * 2.0f));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    m = glm::translate(glm::mat4(1.0f), glm::vec3(size, height - 0.2f, 0.0f));
    m = glm::scale(m, glm::vec3(railThickness, railThickness, size * 2.0f));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    m = glm::translate(glm::mat4(1.0f), glm::vec3(size, height / 2.0f, 0.0f));
    m = glm::scale(m, glm::vec3(railThickness, railThickness, size * 2.0f));
    standardShader->setMat4("model", m); boxMesh->draw();

    // Front edge rails (z = -size) - Split into two
    float segLen = size - 10.0f;
    float segL = -size + segLen/2.0f;
    float segR = 10.0f + segLen/2.0f;

    m = glm::translate(glm::mat4(1.0f), glm::vec3(segL, height - 0.2f, -size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
    m = glm::translate(glm::mat4(1.0f), glm::vec3(segL, height / 2.0f, -size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    m = glm::translate(glm::mat4(1.0f), glm::vec3(segR, height - 0.2f, -size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
    m = glm::translate(glm::mat4(1.0f), glm::vec3(segR, height / 2.0f, -size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    // Back edge rails (z = size) - Split into two
    m = glm::translate(glm::mat4(1.0f), glm::vec3(segL, height - 0.2f, size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
    m = glm::translate(glm::mat4(1.0f), glm::vec3(segL, height / 2.0f, size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
    
    m = glm::translate(glm::mat4(1.0f), glm::vec3(segR, height - 0.2f, size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
    m = glm::translate(glm::mat4(1.0f), glm::vec3(segR, height / 2.0f, size));
    m = glm::scale(m, glm::vec3(segLen, railThickness, railThickness));
    standardShader->setMat4("model", m); boxMesh->draw();
}

void Park::draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos, float yaw) {
    standardShader->use();
    standardShader->setMat4("view", view);
    standardShader->setMat4("projection", projection);
    standardShader->setVec3("viewPos", viewPos);

    glm::vec3 lightPos(20.0f, 50.0f, 20.0f);
    standardShader->setVec3("lightPos", lightPos);
    standardShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.9f));
    standardShader->setVec3("fogColor", glm::vec3(0.53f, 0.81f, 0.92f));

    glm::mat4 model = glm::mat4(1.0f);
    standardShader->setMat4("model", model);
    standardShader->setVec2("texScale", glm::vec2(1.0f, 1.0f));
    standardShader->setBool("useTexture", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texGrass);
    groundMesh->draw();

    // --- Massively Expanded Path Network ---
    float pathWidth = 10.0f;
    
    // Vertical Axis (runs completely from front to back)
    drawPathStraight(glm::vec3(0.0f, 0.0f, -380.0f), glm::vec3(0.0f, 0.0f, 380.0f), pathWidth);
    
    // Horizontal Axis (runs completely left to right)
    drawPathStraight(glm::vec3(-380.0f, 0.0f, 0.0f), glm::vec3(380.0f, 0.0f, 0.0f), pathWidth);

    // Center Circular Plaza (radius 40m)
    drawPathCurve(glm::vec3(0.0f, 0.0f, 0.0f), 40.0f, 0.0f, glm::radians(360.0f), 12.0f);

    // Massive Outer Ring Road (radius 250m)
    drawPathCurve(glm::vec3(0.0f, 0.0f, 0.0f), 250.0f, 0.0f, glm::radians(360.0f), pathWidth);

    // Branch Path leading to the Sign
    drawPathStraight(glm::vec3(0.0f, 0.0f, -80.0f), glm::vec3(80.0f, 0.0f, -80.0f), 6.0f);

    // Branch Paths to Restaurants and Shop
    drawPathStraight(glm::vec3(0.0f, 0.0f, 150.0f), glm::vec3(250.0f, 0.0f, 150.0f), 6.0f); // East Rest
    drawPathStraight(glm::vec3(-100.0f, 0.0f, 0.0f), glm::vec3(-100.0f, 0.0f, -25.0f), 6.0f); // West Rest
    drawPathStraight(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(50.0f, 0.0f, 50.0f), 4.0f); // Shop

    // --- Draw Trees ---
    standardShader->setBool("useTexture", true);
    for (const auto& tree : trees) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(tree.x, tree.y + 1.0f, tree.z));
        standardShader->setMat4("model", model);
        glBindTexture(GL_TEXTURE_2D, texBark);
        trunkMesh->draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(tree.x, tree.y + 3.2f, tree.z));
        standardShader->setMat4("model", model);
        glBindTexture(GL_TEXTURE_2D, texLeaves);
        leafMesh->draw();
    }
    
    // Draw 3D Minecraft-style Landmark Sign in the mountainous grass!
    glm::vec3 signPos(80.0f, Mesh::getTerrainHeight(80.0f, -80.0f), -80.0f);
    drawSign(signPos, glm::radians(-90.0f));
    
    // Draw the Water Pond at (-100, -100)
    standardShader->setBool("useTexture", true);
    standardShader->setVec2("texScale", glm::vec2(1.0f, 1.0f));
    standardShader->setVec3("solidColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Neutral
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texWater);
    
    glm::mat4 pondModel = glm::mat4(1.0f);
    pondModel = glm::translate(pondModel, glm::vec3(-100.0f, -0.5f, -100.0f));
    standardShader->setMat4("model", pondModel);
    waterMesh->draw();
    
    // Draw Restaurants!
    glm::vec3 eastRestPos(250.0f, Mesh::getTerrainHeight(250.0f, 150.0f), 150.0f);
    drawRestaurant(eastRestPos, glm::radians(180.0f)); // Facing North

    glm::vec3 westRestPos(-100.0f, Mesh::getTerrainHeight(-100.0f, -30.0f), -30.0f);
    drawRestaurant(westRestPos, glm::radians(0.0f)); // Facing South towards path
    
    // Draw Small Coffee Shop
    glm::vec3 shopPos(50.0f, Mesh::getTerrainHeight(50.0f, 50.0f), 50.0f);
    drawShop(shopPos, glm::radians(180.0f)); // Facing North towards path
    
    // Draw Border Fence (expanded to 380x380)
    drawFence(380.0f);
    
    // Draw North Entrance Reception
    drawBuilding(glm::vec3(-12.0f, 0.0f, -380.0f));
    drawBuilding(glm::vec3(12.0f, 0.0f, -380.0f));
    
    // Draw South Entrance Reception
    drawBuilding(glm::vec3(-12.0f, 0.0f, 380.0f));
    drawBuilding(glm::vec3(12.0f, 0.0f, 380.0f));
    
    drawHUD(viewPos, yaw);
}

void Park::drawRestaurant(glm::vec3 position, float rotationY) {
    standardShader->use();
    standardShader->setBool("useTexture", true);
    standardShader->setVec2("texScale", glm::vec2(2.0f, 1.0f)); // Tile horizontally 2x
    standardShader->setVec3("solidColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texRestaurant);
    
    // Main building block (30m wide, 10m high, 15m deep)
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, position + glm::vec3(0.0f, 5.0f, 0.0f));
    m = glm::rotate(m, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, glm::vec3(30.0f, 10.0f, 15.0f));
    standardShader->setMat4("model", m);
    boxMesh->draw();
    
    // Overhanging flat roof (dark grey)
    standardShader->setBool("useTexture", false);
    standardShader->setVec3("solidColor", glm::vec3(0.2f, 0.2f, 0.2f));
    m = glm::mat4(1.0f);
    m = glm::translate(m, position + glm::vec3(0.0f, 10.5f, 0.0f));
    m = glm::rotate(m, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, glm::vec3(32.0f, 1.0f, 18.0f)); // Slightly overhangs the building
    standardShader->setMat4("model", m);
    boxMesh->draw();
}

void Park::drawShop(glm::vec3 position, float rotationY) {
    standardShader->use();
    standardShader->setBool("useTexture", true);
    standardShader->setVec2("texScale", glm::vec2(1.0f, 1.0f));
    standardShader->setVec3("solidColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texShop);
    
    // Main shop building (12m wide, 6m high, 8m deep)
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, position + glm::vec3(0.0f, 3.0f, 0.0f));
    m = glm::rotate(m, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, glm::vec3(12.0f, 6.0f, 8.0f));
    standardShader->setMat4("model", m);
    boxMesh->draw();
    
    // Awning/Roof
    standardShader->setBool("useTexture", false);
    standardShader->setVec3("solidColor", glm::vec3(0.1f, 0.1f, 0.1f));
    m = glm::mat4(1.0f);
    m = glm::translate(m, position + glm::vec3(0.0f, 6.2f, 1.0f)); // shifted slightly forward
    m = glm::rotate(m, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, glm::vec3(13.0f, 0.5f, 10.0f)); 
    standardShader->setMat4("model", m);
    boxMesh->draw();
}

void Park::drawHUD(const glm::vec3& viewPos, float yaw) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    uiShader->use();
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    uiShader->setMat4("projection", projection);

    // Map background: dark transparent box
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(580.0f, 380.0f, 0.0f));
    model = glm::scale(model, glm::vec3(200.0f, 200.0f, 1.0f));
    uiShader->setMat4("model", model);
    uiShader->setVec4("color", glm::vec4(0.1f, 0.1f, 0.1f, 0.6f));
    uiMesh->draw();

    auto drawDot = [&](float worldX, float worldZ, float size, glm::vec4 color) {
        float mapX = 580.0f + (worldX + 400.0f) * (200.0f / 800.0f);
        float mapY = 380.0f + (worldZ + 400.0f) * (200.0f / 800.0f);
        
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, glm::vec3(mapX - size/2.0f, mapY - size/2.0f, 0.0f));
        m = glm::scale(m, glm::vec3(size, size, 1.0f));
        uiShader->setMat4("model", m);
        uiShader->setVec4("color", color);
        uiMesh->draw();
    };

    // Draw Entrance (Yellow)
    drawDot(0.0f, -380.0f, 8.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    // Draw Sign (Red)
    drawDot(80.0f, -80.0f, 6.0f, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));

    // Draw Pond (Blue)
    drawDot(-100.0f, -100.0f, 8.0f, glm::vec4(0.2f, 0.5f, 1.0f, 1.0f));
    
    // Draw Restaurants (Orange)
    drawDot(250.0f, 150.0f, 7.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
    drawDot(-100.0f, -30.0f, 7.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

    // Draw Shop (Purple)
    drawDot(50.0f, 50.0f, 6.0f, glm::vec4(0.8f, 0.2f, 0.8f, 1.0f));

    // Draw Player (White Arrow)
    float mapX = 580.0f + (viewPos.x + 400.0f) * (200.0f / 800.0f);
    float mapY = 380.0f + (viewPos.z + 400.0f) * (200.0f / 800.0f);
    
    glm::mat4 playerModel = glm::mat4(1.0f);
    playerModel = glm::translate(playerModel, glm::vec3(mapX, mapY, 0.0f));
    playerModel = glm::rotate(playerModel, glm::radians(yaw + 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    playerModel = glm::scale(playerModel, glm::vec3(10.0f, 12.0f, 1.0f)); // visible arrow
    uiShader->setMat4("model", playerModel);
    uiShader->setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    arrowMesh->draw();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
