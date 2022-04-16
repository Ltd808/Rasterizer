#include "Entity.h"

Entity::Entity(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;
	transform = new Transform();
}

Entity::~Entity()
{
	delete transform;
}

void Entity::Draw(Camera* camera)
{
	// Sets shader program as active and sets uniforms before drawing
	//material->PrepareMaterial(transform, camera);
	mesh->Draw();
}