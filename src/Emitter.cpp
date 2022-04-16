#include "Emitter.h"

Emitter::Emitter(int maxParticles, int particlesPerSecond, float particleLifetime, int bufferIndex, Shader* shader, Texture* texture)
{
	this->maxParticles = maxParticles;
	this->particlesPerSecond = particlesPerSecond;
	this->particleLifetime = particleLifetime;
	this->bufferIndex = bufferIndex;
	this->particleShader = shader;
	this->particleTexture = texture;

	// Calculate emission rate
	secondsPerParticle = 1.0f / particlesPerSecond;

	// Set up emission and lifetime stats
	timeSinceLastEmit = 0.0f;
	liveParticleCount = 0;
	indexFirstAlive = 0;
	indexFirstDead = 0;

	particleData = new Particle[maxParticles];
	//ZeroMemory(particles, sizeof(Particle) * maxParticles);

	// Create an index buffer for particle drawing
	// indices as if we had two triangles per particle
	indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}

	// Copy indices to GPU
	// Generate VAO, VBO, EBO
	glGenVertexArrays(1, &particleVAO);
	glBindVertexArray(particleVAO);

	// no need for vertex buffer, might not even need this, test removing at some point
	glGenBuffers(1, &particleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &particleEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particleEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * maxParticles * 6, &indices[0], GL_STATIC_DRAW);

	// Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Cleanup
	delete[] indices;

	// Initialize SSBO
	glGenBuffers(1, &particleDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * maxParticles, &particleData[0], GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferIndex, particleDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	transform = new Transform();
}

Emitter::~Emitter()
{
	// Clean up particle array
	delete[] particleData;
}

void Emitter::Draw(Camera* camera, float currentTime)
{
	// Activate shader program
	particleShader->Use();

	// Set texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particleTexture->ID);

	// Set uniforms
	particleShader->SetMat4("view", camera->GetViewMatrix());
	particleShader->SetMat4("projection", camera->GetProjectionMatrix());
	particleShader->SetFloat("currentTime", currentTime);

	// Need to set binding for shader block
	GLuint block_index = glGetProgramResourceIndex(particleShader->ID, GL_SHADER_STORAGE_BLOCK, "particleData");
	glShaderStorageBlockBinding(particleShader->ID, block_index, bufferIndex);
	
	//glShaderStorageBlockBinding(program, block_index, 80);

	// Bind the vertex array object
	glBindVertexArray(particleVAO);

	// Ready to draw
	//glDrawElementsBaseVertex(GL_TRIANGLES, liveParticleCount * 6, GL_UNSIGNED_INT, &indices[0], 0);
	glDrawElements(GL_TRIANGLES, liveParticleCount * 6, GL_UNSIGNED_INT, 0);

	// Unbind the vertex array
	glBindVertexArray(0);
}

void Emitter::Update(float DeltaTime, float currentTime)
{
	// Anything to update?
	if (liveParticleCount > 0)
	{
		// Update all particles - Check cyclic buffer first
		if (indexFirstAlive < indexFirstDead)
		{
			// First alive is BEFORE first dead, so the "living" particles are contiguous

			// First alive is before first dead, so no wrapping
			for (int i = indexFirstAlive; i < indexFirstDead; i++)
			{
				UpdateSingleParticle(currentTime, i);
			}

		}
		else if (indexFirstDead < indexFirstAlive)
		{
			// First alive is AFTER first dead, so the "living" particles wrap around

			// Update first half (from firstAlive to max particles)
			for (int i = indexFirstAlive; i < maxParticles; i++)
			{
				UpdateSingleParticle(currentTime, i);
			}

			// Update second half (from 0 to first dead)
			for (int i = 0; i < indexFirstDead; i++)
			{
				UpdateSingleParticle(currentTime, i);
			}
		}
		else
		{
			// First alive is EQUAL TO first dead, so they're either all alive or all dead
			for (int i = 0; i < maxParticles; i++)
			{
				UpdateSingleParticle(currentTime, i);
			}			
		}
	}

	// Add to the time
	timeSinceLastEmit += DeltaTime;

	// Enough time to emit?
	while (timeSinceLastEmit > secondsPerParticle)
	{
		EmitParticle(currentTime);
		timeSinceLastEmit -= secondsPerParticle;
	}

	// SSBO update, copy CPU to GPU
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleDataSSBO);
	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	
	// How are living particles arranged in the buffer?
	if (indexFirstAlive < indexFirstDead)
	{
		// Only copy from FirstAlive -> FirstDead
		memcpy(
			p, // Destination = start of particle buffer
			particleData + indexFirstAlive, // Source = particle array, offset to first living particle
			sizeof(Particle) * liveParticleCount); // Amount = number of particles (measured in BYTES!)
	}
	else
	{
		// Copy from 0 -> FirstDead
		memcpy(
			p, // Destination = start of particle buffer
			particleData,    // Source = start of particle array
			sizeof(Particle) * indexFirstDead); // Amount = particles up to first dead (measured in BYTES!)

		// ALSO copy from FirstAlive -> End
		memcpy(
			(void*)((Particle*)p + indexFirstDead), // Destination = particle buffer, AFTER the data we copied in previous memcpy()
			particleData + indexFirstAlive,  // Source = particle array, offset to first living particle
			sizeof(Particle) * (maxParticles - indexFirstAlive)); // Amount = number of living particles at end of array (measured in BYTES!)
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Emitter::EmitParticle(float currentTime)
{
	// Any left to spawn?
	if (liveParticleCount == maxParticles)
	{
		return;
	}

	// Which particle is spawning?
	int spawnedIndex = indexFirstDead;

	// Update the spawn time of the first dead particle
	particleData[spawnedIndex].emitTime = currentTime;

	// Random initial pos
	particleData[spawnedIndex].startPosition = glm::vec4(transform->GetPosition() + glm::vec3(RandomRange(0, 1)), 1);// +glm::vec3(RandomRange(0, 1)), 1);

	// Random initial color
	//float rColor = 0.5f + ((rand() % 100) / 100.0f);
	//particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	
	// Increment the first dead particle (since it's now alive)
	indexFirstDead++;
	indexFirstDead %= maxParticles; // Wrap

	// One more living particle
	liveParticleCount++;
}

void Emitter::UpdateSingleParticle(float currentTime, int index)
{
	float age = currentTime - particleData[index].emitTime;

	// Update and check for death
	if (age >= particleLifetime)
	{
		// Recent death, so retire by moving alive count (and wrap)
		indexFirstAlive++;
		indexFirstAlive %= maxParticles;
		liveParticleCount--;
	}
}