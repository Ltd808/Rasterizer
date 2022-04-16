#include "Transform.h"

Transform::Transform()
{
	// Init vectors and matrices
	model = glm::mat4(1.0f);

	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);

	forward = glm::vec3(1, 0, 0);
	right = glm::vec3(0, 1, 0);
	up = glm::vec3(0, 0, 1);

	parent = nullptr;
	areMatricesDirty = false;
}

void Transform::UpdateMatrices()
{
	if (areMatricesDirty)
	{
		// Update model Matrix
		model = glm::mat4(1.0f);

		// If this object has a parent apply that transform
		if (parent)
		{
			model *= parent->GetModelMatrix();
		}

		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, scale);

		// Update directional vectors from new model matrix, could be optimized
		glm::mat4 inverted = glm::inverse(model);
		up = normalize(glm::vec3(inverted[2]));
		forward = normalize(glm::vec3(inverted[0]));
		right = normalize(glm::vec3(inverted[1]));

		areMatricesDirty = false;
	}
}

void Transform::SetPosition(glm::vec3 newPosition) 
{ 
	position = newPosition; 
	areMatricesDirty = true; 
	SetChildrenMatricesDirty(); 
}
void Transform::SetRotation(glm::vec3 newRotation) 
{ 
	rotation = newRotation; 
	areMatricesDirty = true;
	SetChildrenMatricesDirty();
}
void Transform::SetScale(glm::vec3 newScale) 
{ 
	scale = newScale; 
	areMatricesDirty = true; 
	SetChildrenMatricesDirty(); 
}



void Transform::Move(glm::vec3 vectorToAdd)
{
	position += vectorToAdd;

	// Update matrices
	areMatricesDirty = true;
	SetChildrenMatricesDirty();
}

void Transform::Rotate(glm::vec3 rotationToAdd)
{
	rotation += rotationToAdd;

	// Update matrices
	areMatricesDirty = true;
	SetChildrenMatricesDirty();
}

void Transform::SetParent(Transform* newParent)
{
	newParent->children.push_back(this);
	parent = newParent;
}

int Transform::GetChildIndex(Transform* child) 
{ 
	// Null check
	if (!child)
	{
		return -1;
	}

	// Search
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i] == child)
		{
			return i;
		}
	}

	// Not found
	return -1;
}

void Transform::AddChild(Transform* child, bool isRelative) 
{ 
	// Check if child is null
	if (!child)
	{
		return;
	}
	
	// Check if already a child
	if (GetChildIndex(child) != -1)
	{
		return;
	}

	//if (isRelative)
	//{
	//	glm::mat4 parentModel = GetModelMatrix();
	//	glm::mat4 childModel = child->GetModelMatrix();

	//	// Get inverse of parent to correct
	//	glm::mat4 parentModelInverse = glm::inverse(parentModel);

	//	// Apply inverse of parent 
	//	glm::mat4 childModelRelative = childModel * parentModelInverse;

	//	child->SetTransformFromMatrix(childModelRelative);
	//}

	children.push_back(child);
	child->parent = this; 

	// Set transforms as dirty
	child->areMatricesDirty = true;
	child->SetChildrenMatricesDirty();
} 
void Transform::RemoveChild(Transform* child, bool isRelative) 
{ 
	//if (isRelative)
	//{
	//	glm::mat4 childModel = child->GetModelMatrix();

	//	child->SetTransformFromMatrix(childModel);
	//}

	child->parent = nullptr;
	children.erase(children.begin() + GetChildIndex(child)); 
}

//void Transform::SetTransformFromMatrix(glm::mat4 model)
//{
//	// Not using skew and perspective, quat is temp
//	glm::vec3 skew;
//	glm::vec4 perspective;
//	glm::quat rotationQuat;
//
//	glm::decompose(model, scale, rotationQuat, position, skew, perspective);
//
//	rotation = glm::eulerAngles(rotationQuat);
//}

glm::mat4 Transform::GetModelMatrix()
{
	UpdateMatrices();
	return model;
}

void Transform::SetChildrenMatricesDirty()
{
	for (Transform* c : children)
	{
		c->areMatricesDirty = true;
		c->SetChildrenMatricesDirty();
	}
}