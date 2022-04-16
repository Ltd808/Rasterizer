#pragma once
#include <algorithm>

#include<stb/stb_image.h>

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class Sky
{
public:
	Sky(Mesh* mesh, Shader* shader, Shader* irradianceShader, Shader* specularShader, Shader* BRDFShader, std::vector<std::string> filePaths);

	void Draw(Camera* camera);

	GLuint GetIrradianceMap() { return irradianceMap; }
	GLuint GetConvolvedSpecularMap() { return convolvedSpecularMap; }
	GLuint GetBRDFLookUpTexture() { return BRDFLookUpMap; }
	//unsigned int GetTotalMipLevels() { return totalMipLevels; }

	void CreateIrradianceMap(GLuint FBO, GLuint RBO);
	void CreateConvolvedSpecularMap(GLuint FBO, GLuint RBO);
	void CreateBRDFLookUpTexture(GLuint FBO, GLuint RBO);

	void RenderQuad();

private:
	Mesh* mesh;
	Shader* shader;
	Shader* irradianceShader;
	Shader* specularShader;
	Shader* BRDFShader;

	// Textures
	GLuint environmentMap;
	GLuint irradianceMap;
	GLuint convolvedSpecularMap;
	GLuint BRDFLookUpMap;

	int cubeMapRes; // store cubemap res
	//int totalMipLevels = 0;
	//const GLuint mipLevelsToSkip = 3;
	const GLuint IBLMapRes = 32;
	const GLuint lookUpRes = 128;
};

