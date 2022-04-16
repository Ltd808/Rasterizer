#pragma once
#include <iostream>

#include <glad/glad.h>
#include<stb/stb_image.h>

class Texture
{
public:

	GLuint ID; // OpenGL texture reference

	// Load texture from path
	Texture(const char* filePath);
	~Texture();
};

