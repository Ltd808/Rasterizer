#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "Scene.h"

class Renderer
{
public:
	// Add sky later
	Renderer(int width, int height, Scene* scene, GLFWwindow* window);

	void PostResize(int width, int height);
	void Render(Camera* camera, float DeltaTime, float currentTime);

	void RenderScene(bool isLight);

	// Setters
	void SetIsPostProcess(bool isActive) { isPostProcess = isActive; }

	// Getters
	bool GetIsPostProcess() { return isPostProcess; }
	GLuint GetColorTexture() { return colorTexture; }
	GLuint GetNormalTexture() { return normalTexture; }
	GLuint GetDepthTexture() { return depthTexture; }

	GLuint GetDepthMap() { return depthMap; }
	glm::vec2 refractionScale = glm::vec2(1,1);

private:
	Scene* scene;
	GLFWwindow* window;

	GLuint FBO;
	GLuint RBO;
	GLuint colorTexture;
	GLuint normalTexture;
	GLuint depthTexture;

	// For shadow maps
	GLuint depthFBO;
	GLuint depthMap;
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;

	glm::mat4 lightProjection;
	glm::mat4 lightView;
	glm::mat4 lightSpaceMatrix;
	
	bool isPostProcess = true;

	int width;
	int height;

	void DrawPointLights(Camera* camera);
};

