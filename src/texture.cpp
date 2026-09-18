#include "texture.h"

// Define stb_image implementation exactly once in the project
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

GLuint loadTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    
    // OpenGL expects the 0.0 coordinate on the Y-axis to be on the bottom, 
    // but images usually have 0.0 at the top.
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        
        // Load data into OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        // Set texture wrapping to GL_REPEAT (usually default)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // Set texture filtering parameters without mipmaps to ensure texture completeness
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    } else {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
    }
    
    stbi_image_free(data);
    return textureID;
}
