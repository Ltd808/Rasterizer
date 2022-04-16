#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>

#include "Renderer.h"

//-------------------------------------------------------------------------------------------
// Using code from https://learnopengl.com/ for PBR, IBL, shader loading, basic project setup, shadow mapping
// -------------------------------------------------------------------------------------------------
// Using code from Chris Cascioli, https://github.com/vixorien/ggp-advanced-demos, for emitter class
// -------------------------------------------------------------------------------------------------

// Window info
int width = 1280;
int height = 720;

// Scene for holding object refs
Scene* scene;

// Renderer for drawing objects
Renderer* renderer;

// GLFW window object
GLFWwindow* window;

// Input
bool isWireframe;
bool guiWantsInput;
double savedMouseX;
double savedMouseY;
bool firstClick = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// For debug context
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);

// For checking held down inputs
void ProcessInput(GLFWwindow* window);

// Callback for window resize
void ResizeCallback(GLFWwindow* window, int width, int height);

// Callback for key presses
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Callback for scroll
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Update ImGui
void UpdateImGui(ImGuiIO& io);

// Update ImGui helper
void DrawEntityNode(std::pair<std::string, Entity*> element);

int main()
{
	// Check for memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialize GLFW
	glfwInit();

	// Configure GLFW for OpenGL 3.3 Core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);	// 4x antialiasing
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // Debug context, comment this out for better performance

	// Creating a window object
	window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make window the current context
	glfwMakeContextCurrent(window);

	// glfw callback functions
	glfwSetFramebufferSizeCallback(window, ResizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// Initialize GLAD to load opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Enable OpenGL debug context if context allows for debug context
	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Get io
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)width;
	io.DisplaySize.y = (float)height;

	// ImGui style
	ImGui::StyleColorsDark();

	// Init ImGui Glfw and OpenGL
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Scene loads all the objects
	// Should probably make a real asset manager at some point
	scene = new Scene(width, height, window);

	renderer = new Renderer(width, height, scene, window);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Timer
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		if (!io.WantCaptureMouse)
		{
			ProcessInput(window);
		}

		// Update
		UpdateImGui(io);
		scene->Update(deltaTime, currentFrame);

		// Draw
		renderer->Render(scene->GetCamera(), deltaTime, currentFrame);

		// GLFW events
		glfwPollEvents();
	}

	// ImGui clean up
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete objects
	delete scene;
	delete renderer;

	// Delete window resources
	glfwDestroyWindow(window);

	// Delete GLFW resources
	glfwTerminate();

	return 0;
}

void ProcessInput(GLFWwindow* window)
{
	// Escape closes window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	Camera* camera = scene->GetCamera();
	Transform* cameraTransform = camera->GetTransform();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraTransform->Move(cameraTransform->GetForward() * camera->GetSpeed() * deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraTransform->Move(-cameraTransform->GetRight() * camera->GetSpeed() * deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraTransform->Move(-cameraTransform->GetForward() * camera->GetSpeed() * deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraTransform->Move(cameraTransform->GetRight() * camera->GetSpeed() * deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		cameraTransform->Move(cameraTransform->GetUp() * camera->GetSpeed() * deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		cameraTransform->Move(-cameraTransform->GetUp() * camera->GetSpeed() * deltaTime);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (firstClick)
		{
			// Save original cursor pos
			glfwGetCursorPos(window, &savedMouseX, &savedMouseY);
			firstClick = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			double mouseX;
			double mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			float xoffset = savedMouseX - mouseX;
			float yoffset = savedMouseY - mouseY;

			xoffset *= camera->GetSensitivity() * deltaTime;
			yoffset *= camera->GetSensitivity() * deltaTime;

			cameraTransform->Rotate(glm::vec3(0, yoffset, xoffset));

			glm::vec3 rotation = cameraTransform->GetRotation();
			if (rotation.y > 89.0f)
			{
				rotation.y = 89.0f;
				cameraTransform->SetRotation(rotation);
			}
			if (rotation.y < -89.0f)
			{
				rotation.y = -89.0f;
				cameraTransform->SetRotation(rotation);
			}

			glfwSetCursorPos(window, savedMouseX, savedMouseY);
		}

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// tell GLFW to not capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (!firstClick)
		{
			firstClick = true;
		}
	}
}

void ResizeCallback(GLFWwindow* window, int width, int height)
{
	// Update viewport and camera
	glViewport(0, 0, width, height);

	scene->GetCamera()->SetAspect((float)width / (float)height);
	scene->GetCamera()->UpdateProjectionMatrix();

	renderer->PostResize(width, height);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// X key toggles wireframe
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		isWireframe = !isWireframe;

		if (isWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	// Toggle post process
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		renderer->SetIsPostProcess(!renderer->GetIsPostProcess());
	}

	// Cycle through skyboxes
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		unsigned int index = scene->GetSkyIndex();
		if (index == 0)
		{
			index = scene->GetSkyCount() - 1;
		}
		else
		{
			index--;
		}
		scene->SetSkyIndex(index);
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		unsigned int index = scene->GetSkyIndex();
		if (index == scene->GetSkyCount() - 1)
		{
			index = 0;
		}
		else
		{
			index++;
		}
		scene->SetSkyIndex(index);
	}
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Set speed with vertical scroll, **find out what xoffset is for at some point**
	float newSpeed = scene->GetCamera()->GetSpeed() + (float)yoffset;

	if (newSpeed < 0.1f)
	{
		newSpeed = 0.1f;
	}

	if (newSpeed > 10.0f)
	{
		newSpeed = 10.0f;
	}

	scene->GetCamera()->SetSpeed(newSpeed);
}

void UpdateImGui(ImGuiIO& io)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//dont taint own own input
	//disable input

	// Create engine stats window
	ImGui::Begin("Engine Info");  // Everything after is part of the window
	ImGui::Text("FPS: %f", io.Framerate);
	ImGui::Text("Window width: %i", width);
	ImGui::Text("Window height: %i", height);
	ImGui::Text("Controls:");
	ImGui::Text("W/A/S/D/Space/LCtrl - Movement");
	ImGui::Text("LMouseButton/Drag - Look around");
	ImGui::Text("Scroll - Adjust speed");
	ImGui::Text("Left/Right - Cycle skyboxes");
	ImGui::Text("X - Toggle wireframe");
	ImGui::Text("C - Toggle post-processing **this will cause refractive objects to not draw**");
	ImGui::End();

	// Create scene object list
	ImGui::Begin("Entities");
	for (std::pair<std::string, Entity*> element : scene->GetEntities())
	{
		DrawEntityNode(element);
	}
	ImGui::End();

	// Debug textures
	ImGui::Begin("Texture Debug");
	//ImGui::Image((void*)(intptr_t)scene->GetTexture("Particle")->ID, ImVec2(256, 256));
	ImGui::Image((void*)(intptr_t)renderer->GetDepthMap(), ImVec2(256, 256));
	//ImGui::Image((void*)(intptr_t)renderer->GetColorTexture(), ImVec2(128, 128));
	//ImGui::Image((void*)(intptr_t)renderer->GetNormalTexture(), ImVec2(128, 128));
	//ImGui::Image((void*)(intptr_t)renderer->GetDepthTexture(), ImVec2(128, 128));
	ImGui::End();

	ImGui::Begin("Refraction");
	ImGui::DragFloat2("RefractionScale", (float*)&renderer->refractionScale);
	ImGui::End();
	//ImGui::ShowDemoWindow();
}

void DrawEntityNode(std::pair<std::string, Entity*> element)
{
	if (ImGui::TreeNode(element.first.c_str()))
	{
		// Matrices will only update if we set as dirty
		// While this tree node is open the matrices for this entity will update
		element.second->GetTransform()->SetMatricesDirty(true);

		// Position
		ImGui::DragFloat3("Position", &element.second->GetTransform()->GetPosition().x);

		// Rotation
		ImGui::DragFloat3("Rotation", &element.second->GetTransform()->GetRotation().x);

		// Scale
		ImGui::DragFloat3("Scale", &element.second->GetTransform()->GetScale().x);

		ImGui::TreePop();
	}
}

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

