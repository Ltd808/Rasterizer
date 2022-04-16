#include "Renderer.h"

Renderer::Renderer(int width, int height, Scene* scene, GLFWwindow* window)
{
	this->width = width;
	this->height = height;
	this->scene = scene;
	this->window = window;

	// Create depth FBO for shadow maps
	glGenFramebuffers(1, &depthFBO);

	// Create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Bind texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Generate framebuffer object and bind
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Generate render buffer object, bind, attach
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO); // Attach RBO to FBO

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!" << std::endl;
	}

	std::vector<Sky*> skies = scene->GetSkies();
	if (skies.size() > 0)
	{
		for (Sky* sky : skies)
		{
			// Create sky maps
			sky->CreateIrradianceMap(FBO, RBO);
			sky->CreateConvolvedSpecularMap(FBO, RBO);

			//glCullFace(GL_FRONT);
			sky->CreateBRDFLookUpTexture(FBO, RBO);
		}
	}
	//glDeleteRenderbuffers(1, &RBO);
	
	// Resize renderbuffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	// Before rendering, configure the viewport to the original framebuffer's screen dimensions
	glViewport(0, 0, width, height);

	// Replacing texture used in sky
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// Generate color buffer texture, bind, attach
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	
	// Generate normal buffer texture, bind, attach
	glGenTextures(1, &normalTexture);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

	// Generate depth buffer texture, bind, attach
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depthTexture, 0);

	GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffers); 

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!" << std::endl;
	}

	// Enable seamless cubemap sampling for lower mip levels in the convolved specular map
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);	// Set front faces
}

void Renderer::PostResize(int width, int height)
{
	this->width = width;
	this->height = height;
}

void Renderer::Render(Camera* camera, float DeltaTime, float currentTime)
{
	// Need depth buffer for scene
	glEnable(GL_DEPTH_TEST);

	// Clear framebuffer
	glClearColor(0.8f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup depth capture
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	RenderScene(true); // bool controls if light

	// Use custom framebuffer for post process, default for just drawing to screen
	if (isPostProcess)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// reset viewport
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene(false);

	// DRAW SKYBOX
	// Cull front face of skybox
	glCullFace(GL_FRONT);
	// Depth test passes when values are equal to depth buffer's values
	glDepthFunc(GL_LEQUAL);
	// Draw skybox last
	scene->GetSky(scene->GetSkyIndex())->Draw(camera);
	// set depth function back to default
	glDepthFunc(GL_LESS); 

	// DRAW TRANSPARENT OBJECTS
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE); // I think this is additive need to check docs
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Sort and draw entities

	//sort emitters
	glDisable(GL_CULL_FACE); // two sided
	std::map<float, Emitter*> sorted;
	for (std::pair<std::string, Emitter*> element : scene->GetEmitters())
	{
		// Save distance
		float distance = glm::length(camera->GetTransform()->GetPosition() - element.second->transform->GetPosition());
		// Save emitter
		sorted[distance] = element.second;
	}

	// Draw emitters
	for (std::map<float, Emitter*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		it->second->Draw(camera, currentTime);
	}

	// Draw entities
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	// CHECK IF QUAD BACKWARDS, CULLING BACK FACES CAUSES WHITE SCREEN
	// If post processing is enabled, use scene texture / PP shader
	if (isPostProcess)
	{
		// Second pass with default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test

		// Clear default framebuffer
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw quad using our colorbuffer
		glBindTexture(GL_TEXTURE_2D, colorTexture);

		scene->GetShader("PostProcess")->Use();
		//glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, colorTexture);	// use the color attachment texture as the texture of the quad plane
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		scene->GetSky(0)->RenderQuad();

		// Cull back
		glCullFace(GL_BACK);

		//glEnable(GL_DEPTH_TEST);
		// Draw refractive entities entities
		for (std::pair<std::string, Entity*> element : scene->GetEntities())
		{
			if (element.second->GetMaterial()->GetIsRefractive())
			{
				// Shder is activated in prepare material
				element.second->GetMaterial()->PrepareMaterial(
					element.second->GetTransform()->GetModelMatrix(),
					scene->GetCamera()->GetViewMatrix(),
					scene->GetCamera()->GetProjectionMatrix(),
					scene->GetCamera()->GetTransform()->GetPosition(),
					scene->GetSky(scene->GetSkyIndex()),
					depthMap);
				Shader* shader = element.second->GetMaterial()->GetShader();

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, colorTexture);

				shader->SetVec2("screenSize", glm::vec2(width, height));
				shader->SetVec2("refractionScale", refractionScale);

				element.second->Draw(camera);
			}
		}
	}

	// Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap front and back buffers
	glfwSwapBuffers(window);
}

void Renderer::RenderScene(bool isLight)
{
	// Cull back faces of scene objects
	glCullFace(GL_BACK);
	
	if (!isLight)
	{
		DrawPointLights(scene->GetCamera());
	}

	// Set light matrix, dont need to do this everytime
	float near_plane = 1.0f, far_plane = 50.5f;

	glm::vec3 lightPos(0.0f, 0.0f, 10.0f);

	lightProjection = glm::orthoLH(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAtLH(lightPos, lightPos + scene->GetDirectionalLights()[0]->direction, glm::vec3(0.0, 0.0, 1.0));
	lightSpaceMatrix = lightProjection * lightView;

	// Draw entities
	for (std::pair<std::string, Entity*> element : scene->GetEntities())
	{
		if (!element.second->GetMaterial()->GetIsRefractive())
		{
			if(!isLight)
			{
				// Using entity shader
				// Shder is activated in prepare material
				element.second->GetMaterial()->PrepareMaterial(
					element.second->GetTransform()->GetModelMatrix(),
					scene->GetCamera()->GetViewMatrix(),
					scene->GetCamera()->GetProjectionMatrix(),
					scene->GetCamera()->GetTransform()->GetPosition(),
					scene->GetSky(scene->GetSkyIndex()),
					depthMap);

				Shader* shader = element.second->GetMaterial()->GetShader();

				// Get lights from scene
				std::vector<DirectionalLight*> directionalLights = scene->GetDirectionalLights();
				std::vector<PointLight*> pointLights = scene->GetPointLights();

				// Directional lights
				for (size_t i = 0; i < directionalLights.size(); i++)
				{
					std::string number = std::to_string(i);

					shader->SetVec3("directionalLights[" + number + "].direction", directionalLights[i]->direction);
					shader->SetVec3("directionalLights[" + number + "].color", directionalLights[i]->color);
					shader->SetFloat("directionalLights[" + number + "].intensity", directionalLights[i]->intensity);
				}

				// Point lights
				for (size_t i = 0; i < pointLights.size(); i++)
				{
					std::string number = std::to_string(i);

					shader->SetVec3("pointLights[" + number + "].position", pointLights[i]->position);
					shader->SetVec3("pointLights[" + number + "].color", pointLights[i]->color);
					shader->SetFloat("pointLights[" + number + "].intensity", pointLights[i]->intensity);
					shader->SetFloat("pointLights[" + number + "].range", pointLights[i]->range);
				}

				shader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
			}
			else
			{
				// Using depth shader, just set model
				scene->GetShader("SimpleDepth")->Use();
				scene->GetShader("SimpleDepth")->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
				scene->GetShader("SimpleDepth")->SetMat4("model", element.second->GetTransform()->GetModelMatrix());
			}

			element.second->Draw(scene->GetCamera());
		}
	}
}


void Renderer::DrawPointLights(Camera* camera)
{
	// Get resources
	Shader* lightShader = scene->GetShader("Light");
	std::vector<PointLight*> pointLights = scene->GetPointLights();

	// Set shader program
	lightShader->Use();

	// Set camera matrices
	lightShader->SetMat4("view", camera->GetViewMatrix());
	lightShader->SetMat4("projection", camera->GetProjectionMatrix());

	for (PointLight* light : pointLights)
	{
		// Set scale based on range
		float scale = light->range / 10.0f;

		// Build model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, light->position);	
		// No rotation
		model = glm::scale(model, glm::vec3(scale));

		lightShader->SetMat4("model", model);

		// Set up the pixel shader data
		glm::vec3 color = light->color;
		color.x *= light->intensity;
		color.y *= light->intensity;
		color.z *= light->intensity;
		lightShader->SetVec3("color", color);

		// Draw mesh
		scene->GetMesh("Sphere")->Draw();
	}
}
