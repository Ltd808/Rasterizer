#pragma once
#include <algorithm>
#include <vector>

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Transform
{
public:
	Transform();

	// Setters
	void SetPosition(glm::vec3 newPosition);
	void SetRotation(glm::vec3 newRotation);
	void SetScale(glm::vec3 newScale);
	void SetParent(Transform* newParent);
	void SetMatricesDirty(bool isDirty) { areMatricesDirty = isDirty; }

	// Getters
	glm::vec3& GetPosition() { return position; }
	glm::vec3& GetRotation() { return rotation; }
	glm::vec3& GetScale() { return scale; }
	glm::vec3 GetForward() { return forward; }
	glm::vec3 GetRight() { return right; }
	glm::vec3 GetUp() { return up; }
	glm::mat4 GetModelMatrix();
	Transform* GetParent() { return parent; }
	Transform* GetChild(unsigned int index) { return children[index]; }
	int GetChildIndex(Transform* child);
	unsigned int GetChildCount() { return children.size(); }

	// Movement
	void Rotate(glm::vec3 rotationToAdd);
	void Move(glm::vec3 vectorToAdd);

	// Transform hierarchy
	void AddChild(Transform* child, bool isRelative = true);
	void RemoveChild(Transform* child, bool isRelative = true);
	//void SetTransformFromMatrix(glm::mat4 model);
	void SetChildrenMatricesDirty();

	void UpdateMatrices();

private:
	// Transform data
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;

	glm::mat4 model;

	Transform* parent;
	std::vector<Transform*> children;

	bool areMatricesDirty;
};

