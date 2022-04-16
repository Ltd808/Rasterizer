#pragma once
#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

// Vertex data
struct Vertex 
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
	~Mesh();

	//void CreateSphere(float radius, int sectorCount, int stackCount);

	void Draw();

private:
	// Mesh buffers
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	// Vertex data
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

