#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

class Entity
{
public:
	Entity(Mesh* mesh, Material* material);
	~Entity();

	// Per frame
	void Draw(Camera* camera);

	// Getters
	Mesh* GetMesh() { return mesh; }
	Transform* GetTransform() { return transform; }
	Material* GetMaterial() { return material; }

	//bool hasOutline = true;

private:
	Mesh* mesh;
	Transform* transform;
	Material* material;
};

