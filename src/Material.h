#pragma once
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include "Sky.h"

class Material
{
public:
	// For regular entities
	Material(Shader* shader, Texture* albedo, Texture* normal, Texture* metallic, Texture* roughness, bool isPBR = false, bool isRefractive = false);

	// Set shader uniforms
	void PrepareMaterial(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection, glm::vec3 position, Sky* sky, GLuint shadowMap);

	// Getters
	Shader* GetShader() { return shader; }
	bool GetIsRefractive() { return isRefractive; }

private:

	Shader* shader;
	Texture* albedo;
	Texture* normal;
	Texture* metallic;
	Texture* roughness;
	
	float shininess = 32;

	bool isPBR;
	bool isRefractive;
};

