#pragma once
#include <unordered_map>
#include <map>

#include "Entity.h"
#include "Sky.h"
#include "Emitter.h"

// Light count
#define DirectionalLightCount 1
#define PointLightCount 9

struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec3 color;

	float intensity;
};


struct PointLight
{
	glm::vec3 position;
	glm::vec3 color;

	float range;
	float intensity;
};

class Scene
{
public:
	Scene(int width, int height, GLFWwindow* window);
	~Scene();

	void Update(float deltaTime, float currentTime);

	// Getters
	Mesh* GetMesh(std::string name) { return meshes[name]; }
	Texture* GetTexture(std::string name) { return textures[name]; }
	Shader* GetShader(std::string name) { return shaders[name]; }
	Material* GetMaterial(std::string name) { return materials[name]; }
	Entity* GetEntity(std::string name) { return entities[name]; }
	Emitter* GetEmitter(std::string name) { return emitters[name]; }
	Sky* GetSky(int index) { return skies[index]; }
	std::vector<Sky*> GetSkies() { return skies; }
	unsigned int GetSkyIndex() { return skyIndex; }
	unsigned int GetSkyCount() { return skies.size(); }
	Camera* GetCamera() { return camera; }
	std::unordered_map<std::string, Entity*> GetEntities() { return entities; }
	std::unordered_map<std::string, Emitter*> GetEmitters() { return emitters; }
	std::vector<PointLight*> GetPointLights() { return pointLights; }
	std::vector<DirectionalLight*> GetDirectionalLights() { return directionalLights;  }
	GLuint GetPointLightCount() { return pointLights.size(); }

	// Setters
	void SetSkyIndex(unsigned int newIndex) { skyIndex = newIndex; }

	// Adding to maps
	void AddTexture(std::string textureName, Texture* texture) { textures.insert({ textureName, texture }); }
	void AddShader(std::string shaderName, Shader* shader) { shaders.insert({ shaderName, shader }); }
	void AddMaterial(std::string materialName, Material* material) { materials.insert({ materialName, material }); }
	void AddMesh(std::string meshName, Mesh* mesh) { meshes.insert({ meshName, mesh }); }
	void AddEntity(std::string entityName, Entity* entity) { entities.insert({ entityName, entity }); }
	void AddEmmiter(std::string emitterName, Emitter* emitter) { emitters.insert({ emitterName, emitter }); }
	void AddPointLight(PointLight* light) { pointLights.push_back(light); }
	void AddDirectionalLight(DirectionalLight* light) { directionalLights.push_back(light); }

private:
	std::unordered_map<std::string, Mesh*> meshes;
	std::unordered_map<std::string, Texture*> textures;
	std::unordered_map<std::string, Shader*> shaders;
	std::unordered_map<std::string, Material*> materials;
	std::unordered_map<std::string, Entity*> entities;
	std::unordered_map<std::string, Emitter*> emitters;
	std::vector<DirectionalLight*> directionalLights;
	std::vector<PointLight*> pointLights;
	std::vector<Sky*> skies;

	Camera* camera;
	GLFWwindow* window;

	unsigned int skyIndex;

	float totalTime = 0;

	// Helper for random value in range for point light init
	float RandomRange(float min, float max) { return (float)std::rand() / RAND_MAX * (max - min) + min; }

	Mesh* CreateSphere(float radius, int sectorCount, int stackCount);
	Mesh* CreateCube();
};

