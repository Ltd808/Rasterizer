#pragma once
#include <vector>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"


// Particle data
struct Particle
{
	// Position of emission
	glm::vec4 startPosition;

	// Time of emission
	float emitTime;

	// padding
	float padding[3];
};

class Emitter
{
public:
	Emitter(int maxParticles,
			int particlesPerSecond,
			float particleLifetime,
			int bufferIndex,
			Shader* shader, 
			Texture* texture
	);

	~Emitter();
	// Particles -----------------------------------------------

	// Particles
	Particle* particleData;

	unsigned int* indices;

	// Max particles
	int maxParticles;

	// Lifetime of a single particle
	float particleLifetime;

	// Index of where dead particles begin
	int indexFirstDead;

	// Index of where live particles begin
	int indexFirstAlive;

	// Number of live particles
	int liveParticleCount;

	// Emitter -------------------------------------------------

	// Number of articles to emit per second
	int particlesPerSecond;

	// Seconds between each particle emission
	float secondsPerParticle;

	// Time since the last emit
	float timeSinceLastEmit;

	// Rendering -----------------------------------------------

	// Vertex attribute object ID
	GLuint particleVAO;

	GLuint particleVBO;

	GLuint particleEBO;

	// Shader storage buffer object
	GLuint particleDataSSBO;

	// One slot per SSBO
	int bufferIndex;

	// Shader program
	Shader* particleShader;

	// Texture
	Texture* particleTexture;
	
	// Position data
	Transform* transform;

	// Shader params -------------------------------------------


	// Methods -------------------------------------------------

	// Track lifetimes and emit particles
	void Update(float DeltaTime, float currentTime);

	// Draw this emitter
	void Draw(Camera* camera, float currentTime);

	// Helpers
	void EmitParticle(float currentTime);
	void UpdateSingleParticle(float currentTime, int index);
	float RandomRange(float min, float max) { return (float)std::rand() / RAND_MAX * (max - min) + min; }
};

