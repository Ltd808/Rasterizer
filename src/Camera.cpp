#include"Camera.h"

Camera::Camera(glm::vec3 position, float aspect)
{
	transform.SetPosition(position);
	this->aspect = aspect;

	// Update matrices
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::Update(GLFWwindow* window, float deltaTime)
{
	// Update transform
	transform.UpdateMatrices();

	// Update view matrix every frame, optimize later
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	view = glm::lookAtLH(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), transform.GetUp());
}

void Camera::UpdateProjectionMatrix()
{
	// Calculate perspective projection
	projection = glm::perspectiveLH(glm::radians(FOV), aspect, near, far);

	// Use orthographic projection?
	//projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
}
