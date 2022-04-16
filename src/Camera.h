#pragma once
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Transform.h"

class Camera
{
public:
	Camera(glm::vec3 position, float aspect);

	// Updates
	void Update(GLFWwindow* window, float deltaTime);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	// Getters
	glm::mat4x4 GetViewMatrix() { return view; }
	glm::mat4x4 GetProjectionMatrix() { return projection;  }
	Transform* GetTransform() { return &transform; }
	float GetSpeed() { return speed; }
	float GetSensitivity() { return sensitivity; }

	// Setters
	void SetSpeed(float newSpeed) { speed = newSpeed; }
	void SetAspect(float newAspect) { aspect = newAspect; }

private:
	// Camera settings
	float speed = 1.0f;
	float sensitivity = 10.0f;
	float FOV = 45.0f;
	float near = 0.1f;
	float far = 100.0f;	
	float aspect;

	// Matrices
	glm::mat4 view;
	glm::mat4 projection;

	// Transform data
	Transform transform;
};