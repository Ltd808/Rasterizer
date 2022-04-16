#include "Texture.h"


Texture::Texture(const char* filePath)
{
    std::cout << "Loading " << filePath << std::endl;

    // Generate texture object
    glGenTextures(1, &ID);

    // Load image from path
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrComponents, 0);

    // If successful
    if (data)
    {
        // Check format
        GLenum format{};
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        // Bind object
        glBindTexture(GL_TEXTURE_2D, ID);

        // Set texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filter parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Copy data and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(data);
    }
    else
    {
        stbi_image_free(data);
        std::cout << "Texture failed to load at path: " << filePath << std::endl;  
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
}