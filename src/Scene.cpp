#include "Scene.h"

Scene::Scene(int width, int height, GLFWwindow* window)
{ 
    this->window = window;

    // Add camera
    camera = new Camera(glm::vec3(-20, 9, 1), (float)width / (float)height);

    // Add Directional light(s)
    DirectionalLight* dir = new DirectionalLight;
    dir->direction = glm::vec3(1, 1, -1);
    dir->color = glm::vec3(1.0f, 1.0f, 1.0f);
    dir->intensity = 1.0f;

    AddDirectionalLight(dir);

    // Add Point light(s)
    for (size_t i = 0; i < PointLightCount; i++)
    {
        PointLight* point = new PointLight;

        point->position = glm::vec3(RandomRange(-5.0f, 5.0f), RandomRange(0.0f, 18.0f), RandomRange(-5.0f, 5.0f));
        point->color = glm::vec3(RandomRange(0.0f, 1.0f), RandomRange(0.0f, 1.0f), RandomRange(0.0f, 1.0f));

        point->intensity = 1.0f;
        point->range = 4.0f;

        AddPointLight(point);
    }

    // Add shaders
    AddShader("Default", new Shader("Default.vert", "Default.frag"));
    AddShader("DefaultPBR", new Shader("Default.vert", "DefaultPBR.frag"));
    AddShader("Refractive", new Shader("Default.vert", "Refractive.frag"));

    AddShader("Light", new Shader("Light.vert", "Light.frag"));

    AddShader("Sky", new Shader("Sky.vert", "Sky.frag"));
    AddShader("Irradiance", new Shader("Fullscreen.vert", "Irradiance.frag"));
    AddShader("Specular", new Shader("Sky.vert", "SpecularConvolution.frag"));
    AddShader("BRDF", new Shader("BRDF.vert", "BRDFLookUp.frag"));

    AddShader("PostProcess", new Shader("BRDF.vert", "PostProcess.frag"));

    AddShader("Particle", new Shader("Particle.vert", "Particle.frag"));

    AddShader("SimpleDepth", new Shader("Simple.vert", "Empty.frag"));
    
    // Set shader texture units
    GetShader("Default")->Use();
    GetShader("Default")->SetInt("albedoMap", 0);
    GetShader("Default")->SetInt("normalMap", 1);
    GetShader("Default")->SetInt("roughnessMap", 2);
    GetShader("Default")->SetInt("shadowMap", 7);

    GetShader("DefaultPBR")->Use();
    GetShader("DefaultPBR")->SetInt("albedoMap", 0);
    GetShader("DefaultPBR")->SetInt("normalMap", 1);
    GetShader("DefaultPBR")->SetInt("roughnessMap", 2);
    GetShader("DefaultPBR")->SetInt("metallicMap", 3);
    GetShader("DefaultPBR")->SetInt("irradianceMap", 4);
    GetShader("DefaultPBR")->SetInt("specularMap", 5);
    GetShader("DefaultPBR")->SetInt("BRDFLUT", 6);
    GetShader("DefaultPBR")->SetInt("shadowMap", 7);

    GetShader("Refractive")->Use();
    GetShader("Refractive")->SetInt("screenColors", 0);
    GetShader("Refractive")->SetInt("normalMap", 1);

    //GetShader("Sky")->Use();
    //GetShader("Sky")->SetInt("environmentMap", 0);

    //GetShader("Irradiance")->Use();
    //GetShader("Irradiance")->SetInt("environmentMap", 0);

    //GetShader("Specular")->Use();
    //GetShader("Specular")->SetInt("environmentMap", 0);

    //GetShader("PostProcess")->Use();
    //GetShader("PostProcess")->SetInt("screenTexture", 0);

    // Add textures
    AddTexture("BronzeAlbedo", new Texture("Content/Textures/Bronze/bronze_albedo.png"));
    AddTexture("BronzeNormal", new Texture("Content/Textures/Bronze/bronze_normals.png"));
    AddTexture("BronzeMetal", new Texture("Content/Textures/Bronze/bronze_metal.png"));
    AddTexture("BronzeRough", new Texture("Content/Textures/Bronze/bronze_roughness.png"));

    AddTexture("CobbleAlbedo", new Texture("Content/Textures/Cobblestone/cobblestone_albedo.png"));
    AddTexture("CobbleNormal", new Texture("Content/Textures/Cobblestone/cobblestone_normals.png"));
    AddTexture("CobbleMetal", new Texture("Content/Textures/Cobblestone/cobblestone_metal.png"));
    AddTexture("CobbleRough", new Texture("Content/Textures/Cobblestone/cobblestone_roughness.png"));

    AddTexture("FloorAlbedo", new Texture("Content/Textures/Floor/floor_albedo.png"));
    AddTexture("FloorNormal", new Texture("Content/Textures/Floor/floor_normals.png"));
    AddTexture("FloorMetal", new Texture("Content/Textures/Floor/floor_metal.png"));
    AddTexture("FloorRough", new Texture("Content/Textures/Floor/floor_roughness.png"));

    AddTexture("PaintAlbedo", new Texture("Content/Textures/Paint/paint_albedo.png"));
    AddTexture("PaintNormal", new Texture("Content/Textures/Paint/paint_normals.png"));
    AddTexture("PaintMetal", new Texture("Content/Textures/Paint/paint_metal.png"));
    AddTexture("PaintRough", new Texture("Content/Textures/Paint/paint_roughness.png"));

    AddTexture("RoughAlbedo", new Texture("Content/Textures/Rough/rough_albedo.png"));
    AddTexture("RoughNormal", new Texture("Content/Textures/Rough/rough_normals.png"));
    AddTexture("RoughMetal", new Texture("Content/Textures/Rough/rough_metal.png"));
    AddTexture("RoughRough", new Texture("Content/Textures/Rough/rough_roughness.png"));

    AddTexture("ScratchedAlbedo", new Texture("Content/Textures/Scratched/scratched_albedo.png"));
    AddTexture("ScratchedNormal", new Texture("Content/Textures/Scratched/scratched_normals.png"));
    AddTexture("ScratchedMetal", new Texture("Content/Textures/Scratched/scratched_metal.png"));
    AddTexture("ScratchedRough", new Texture("Content/Textures/Scratched/scratched_roughness.png"));

    AddTexture("WoodAlbedo", new Texture("Content/Textures/Wood/wood_albedo.png"));
    AddTexture("WoodNormal", new Texture("Content/Textures/Wood/wood_normals.png"));
    AddTexture("WoodMetal", new Texture("Content/Textures/Wood/wood_metal.png"));
    AddTexture("WoodRough", new Texture("Content/Textures/Wood/wood_roughness.png"));

    AddTexture("GlassNormal", new Texture("Content/Textures/glass_normal.png"));

    AddTexture("ParticleDirt", new Texture("Content/Textures/Particles/dirt.png"));
    AddTexture("ParticleDot", new Texture("Content/Textures/Particles/dot.png"));
    AddTexture("ParticleFlame", new Texture("Content/Textures/Particles/flame.png"));
    AddTexture("ParticleLight", new Texture("Content/Textures/Particles/light.png"));
    AddTexture("ParticleWindow", new Texture("Content/Textures/Particles/window.png"));
    
    // Add materials
    AddMaterial("Bronze", new Material(GetShader("Default"), GetTexture("BronzeAlbedo"), GetTexture("BronzeNormal"), GetTexture("BronzeMetal"), GetTexture("BronzeRough")));
    AddMaterial("Cobble", new Material(GetShader("Default"), GetTexture("CobbleAlbedo"), GetTexture("CobbleNormal"), GetTexture("CobbleMetal"), GetTexture("CobbleRough")));
    AddMaterial("Floor", new Material(GetShader("Default"), GetTexture("FloorAlbedo"), GetTexture("FloorNormal"), GetTexture("FloorMetal"), GetTexture("FloorRough")));
    AddMaterial("Paint", new Material(GetShader("Default"), GetTexture("PaintAlbedo"), GetTexture("PaintNormal"), GetTexture("PaintMetal"), GetTexture("PaintRough")));
    AddMaterial("Rough", new Material(GetShader("Default"), GetTexture("RoughAlbedo"), GetTexture("RoughNormal"), GetTexture("RoughMetal"), GetTexture("RoughRough")));
    AddMaterial("Scratched", new Material(GetShader("Default"), GetTexture("ScratchedAlbedo"), GetTexture("ScratchedNormal"), GetTexture("ScratchedMetal"), GetTexture("ScratchedRough")));
    AddMaterial("Wood", new Material(GetShader("Default"),  GetTexture("WoodAlbedo"), GetTexture("WoodNormal"),  GetTexture("WoodMetal"), GetTexture("WoodRough")));

    AddMaterial("BronzePBR", new Material( GetShader("DefaultPBR"), GetTexture("BronzeAlbedo"), GetTexture("BronzeNormal"), GetTexture("BronzeMetal"), GetTexture("BronzeRough"), true));
    AddMaterial("CobblePBR", new Material(GetShader("DefaultPBR"), GetTexture("CobbleAlbedo"), GetTexture("CobbleNormal"), GetTexture("CobbleMetal"), GetTexture("CobbleRough"), true));
    AddMaterial("FloorPBR", new Material(GetShader("DefaultPBR"), GetTexture("FloorAlbedo"), GetTexture("FloorNormal"), GetTexture("FloorMetal"), GetTexture("FloorRough"), true));
    AddMaterial("PaintPBR", new Material(GetShader("DefaultPBR"), GetTexture("PaintAlbedo"), GetTexture("PaintNormal"), GetTexture("PaintMetal"), GetTexture("PaintRough"), true));
    AddMaterial("RoughPBR", new Material(GetShader("DefaultPBR"), GetTexture("RoughAlbedo"), GetTexture("RoughNormal"), GetTexture("RoughMetal"), GetTexture("RoughRough"), true));
    AddMaterial("ScratchedPBR", new Material(GetShader("DefaultPBR"), GetTexture("ScratchedAlbedo"), GetTexture("ScratchedNormal"), GetTexture("ScratchedMetal"), GetTexture("ScratchedRough"), true));
    AddMaterial("WoodPBR", new Material(GetShader("DefaultPBR"), GetTexture("WoodAlbedo"), GetTexture("WoodNormal"), GetTexture("WoodMetal"), GetTexture("WoodRough"), true));
    
    AddMaterial("Glass", new Material(GetShader("Refractive"), nullptr, GetTexture("GlassNormal"), nullptr, nullptr, false, true));

    // Add emitters
    AddEmmiter("EmitterOne", new Emitter(50, 1, 4, 0, GetShader("Particle"), GetTexture("ParticleDirt")));
    //AddEmmiter("EmitterTwo", new Emitter(50, 2, 4, 1, GetShader("Particle"), GetTexture("ParticleDot")));
    //AddEmmiter("EmitterThree", new Emitter(50, 3, 4, 2, GetShader("Particle"), GetTexture("ParticleFlame")));
    //AddEmmiter("EmitterFour", new Emitter(50, 4, 4, 3, GetShader("Particle"), GetTexture("ParticleLight")));
    //AddEmmiter("EmitterFive", new Emitter(50, 5, 4, 4, GetShader("Particle"), GetTexture("ParticleWindow")));

    // Add meshes
    AddMesh("Sphere", CreateSphere(1, 20, 20));
    AddMesh("Cube", CreateCube());

    std::vector<std::string> blueCloudsTexturePaths =
    {
        "Content/Textures/Skyboxes/BlueClouds/right.png",
        "Content/Textures/Skyboxes/BlueClouds/left.png",
        "Content/Textures/Skyboxes/BlueClouds/up.png",
        "Content/Textures/Skyboxes/BlueClouds/down.png",
        "Content/Textures/Skyboxes/BlueClouds/front.png",
        "Content/Textures/Skyboxes/BlueClouds/back.png"
    };

    std::vector<std::string> pinkCloudsTexturePaths =
    {
        "Content/Textures/Skyboxes/PinkClouds/right.png",
        "Content/Textures/Skyboxes/PinkClouds/left.png",
        "Content/Textures/Skyboxes/PinkClouds/up.png",
        "Content/Textures/Skyboxes/PinkClouds/down.png",
        "Content/Textures/Skyboxes/PinkClouds/front.png",
        "Content/Textures/Skyboxes/PinkClouds/back.png"
    };

    // Add Skies
    skies.push_back(new Sky(
        GetMesh("Cube"),
        GetShader("Sky"),
        GetShader("Irradiance"),
        GetShader("Specular"),
        GetShader("BRDF"),
        blueCloudsTexturePaths));

    //skies.push_back(new Sky(
    //    GetMesh("Cube"),
    //    GetShader("Sky"),
    //    GetShader("Irradiance"),
    //    GetShader("Specular"),
    //    GetShader("BRDF"),
    //    pinkCloudsTexturePaths));

    // Add entities
    // Default shader
    AddEntity("BronzeSphere", new Entity(GetMesh("Sphere"), GetMaterial("Bronze")));
    AddEntity("CobbleSphere", new Entity(GetMesh("Sphere"), GetMaterial("Cobble")));
    AddEntity("FloorSphere", new Entity(GetMesh("Sphere"), GetMaterial("Floor")));
    AddEntity("PaintSphere", new Entity(GetMesh("Sphere"), GetMaterial("Paint")));
    AddEntity("RoughSphere", new Entity(GetMesh("Sphere"), GetMaterial("Rough")));
    AddEntity("ScratchedSphere", new Entity(GetMesh("Sphere"), GetMaterial("Scratched")));
    AddEntity("WoodSphere", new Entity(GetMesh("Sphere"), GetMaterial("Wood")));
    // PBR shader
    AddEntity("BronzeSpherePBR", new Entity(GetMesh("Sphere"), GetMaterial("BronzePBR")));
    AddEntity("CobbleSpherePBR", new Entity(GetMesh("Sphere"), GetMaterial("CobblePBR")));
    AddEntity("FloorSpherePBR", new Entity(GetMesh("Sphere"), GetMaterial("FloorPBR")));
    AddEntity("PaintSpherePBR", new Entity(GetMesh("Sphere"), GetMaterial("PaintPBR")));
    AddEntity("RoughSpherePBR", new Entity(GetMesh("Sphere"), GetMaterial("RoughPBR")));
    AddEntity("ScratchedSpherePBR", new Entity(GetMesh("Sphere"), GetMaterial("ScratchedPBR")));
    AddEntity("WoodSpherePBR", new Entity(GetMesh("Sphere"), GetMaterial("WoodPBR")));

    AddEntity("GlassSphere", new Entity(GetMesh("Sphere"), GetMaterial("Glass")));

    AddEntity("FloorWoodSphere", new Entity(GetMesh("Sphere"), GetMaterial("Wood")));

    // Move entities
    GetEntity("CobbleSphere")->GetTransform()->Move(glm::vec3(0.0f, 3.0f, 0.0f));
    GetEntity("FloorSphere")->GetTransform()->Move(glm::vec3(0.0f, 6.0f, 0.0f));
    GetEntity("PaintSphere")->GetTransform()->Move(glm::vec3(0.0f, 9.0f, 0.0f));
    GetEntity("RoughSphere")->GetTransform()->Move(glm::vec3(0.0f, 12.0f, 0.0f));
    GetEntity("ScratchedSphere")->GetTransform()->Move(glm::vec3(0.0f, 15.0f, 0.0f));
    GetEntity("WoodSphere")->GetTransform()->Move(glm::vec3(0.0f, 18.0f, 0.0f));

    GetEntity("BronzeSpherePBR")->GetTransform()->Move(glm::vec3(0.0f, 0.0f, 3.0f));
    GetEntity("CobbleSpherePBR")->GetTransform()->Move(glm::vec3(0.0f, 3.0f, 3.0f));
    GetEntity("FloorSpherePBR")->GetTransform()->Move(glm::vec3(0.0f, 6.0f, 3.0f));
    GetEntity("PaintSpherePBR")->GetTransform()->Move(glm::vec3(0.0f, 9.0f, 3.0f));
    GetEntity("RoughSpherePBR")->GetTransform()->Move(glm::vec3(0.0f, 12.0f, 3.0f));
    GetEntity("ScratchedSpherePBR")->GetTransform()->Move(glm::vec3(0.0f, 15.0f, 3.0f));
    GetEntity("WoodSpherePBR")->GetTransform()->Move(glm::vec3(0.0f, 18.0f, 3.0f));

    GetEntity("GlassSphere")->GetTransform()->Move(glm::vec3(0.0f, 21.0f, 1.5f));

    GetEntity("FloorWoodSphere")->GetTransform()->Move(glm::vec3(0.0f, 10.0f, -3.0f));
    GetEntity("FloorWoodSphere")->GetTransform()->SetScale(glm::vec3(25, 25, 1));

    // Parent entities
    GetEntity("BronzeSphere")->GetTransform()->AddChild(GetEntity("CobbleSphere")->GetTransform());

    GetEmitter("EmitterOne")->transform->Move(glm::vec3(0, 0, 6));
    //GetEmitter("EmitterTwo")->transform->Move(glm::vec3(0, 3, 6));
    //GetEmitter("EmitterThree")->transform->Move(glm::vec3(0, 6, 6));
    //GetEmitter("EmitterFour")->transform->Move(glm::vec3(0, 9, 6));
    //GetEmitter("EmitterFive")->transform->Move(glm::vec3(0, 12, 6));
}
 
void Scene::Update(float deltaTime, float currentTime)
{
    camera->Update(window, deltaTime);

    for (std::pair<std::string, Emitter*> element : emitters)
    {
        element.second->Update(deltaTime, currentTime);
    }

    totalTime += deltaTime;

    // Do scene stuff
    GetEntity("BronzeSphere")->GetTransform()->SetPosition(glm::vec3(sin(totalTime) * 2, 0.0f, 0.0f));
    GetEntity("BronzeSphere")->GetTransform()->Rotate(glm::vec3(0.0f, 0.0f, 20 * deltaTime));
}

Scene::~Scene()
{
    // Cleanup everything
    for (std::pair<std::string, Entity*> element : entities)
    {
        delete element.second;
    }

    for (std::pair<std::string, Emitter*> element : emitters)
    {
        delete element.second;
    }

    for (std::pair<std::string, Mesh*> element : meshes)
    {
        delete element.second;
    }

    for (std::pair<std::string, Texture*> element : textures)
    {
        delete element.second;
    }

    for (std::pair<std::string, Shader*> element : shaders)
    {
        delete element.second;
    }

    for (std::pair<std::string, Material*> element : materials)
    {
        delete element.second;
    }

    for (PointLight* pointLight : pointLights)
    {
        delete pointLight;
    }

    for (DirectionalLight* directionalLight : directionalLights)
    {
        delete directionalLight;
    }

    for (Sky* sky : skies)
    {
        delete sky;
    }

    delete camera;
}


Mesh* Scene::CreateSphere(float radius, int sectorCount, int stackCount)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float PI = 3.14159265359;
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    int k1, k2;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;    // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);         // r * cos(u)
        z = radius * sinf(stackAngle);          // r * sin(u)

        k1 = i * (sectorCount + 1); // beginning of current stack
        k2 = k1 + sectorCount + 1;  // beginning of next stack

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j, ++k1, ++k2)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;

            vertices.push_back({
                glm::vec3(x, y, z),
                glm::vec2(s, t),
                glm::vec3(nx, ny, nz)
            });

            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return new Mesh(vertices, indices);
}

Mesh* Scene::CreateCube()
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    //vertices.push_back({ glm::vec3(-1.0f, -1.0f,  1.0f) });
    //vertices.push_back({ glm::vec3(1.0f, -1.0f,  1.0f) });
    //vertices.push_back({ glm::vec3(1.0f, -1.0f, -1.0f) });
    //vertices.push_back({ glm::vec3(-1.0f, -1.0f, -1.0f) });
    //vertices.push_back({ glm::vec3(-1.0f,  1.0f,  1.0f) });
    //vertices.push_back({ glm::vec3(1.0f,  1.0f,  1.0f) });
    //vertices.push_back({ glm::vec3(1.0f,  1.0f, -1.0f) });
    //vertices.push_back({ glm::vec3(-1.0f,  1.0f, -1.0f) });

    vertices.push_back({
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec2(-1.0f, -1.0f)
    });

    vertices.push_back({
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec2(1.0f, -1.0f)
    });

    vertices.push_back({
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec2(1.0f, -1.0f)
    });

    vertices.push_back({
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec2(-1.0f, -1.0f)
    });

    vertices.push_back({
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec2(-1.0f,  1.0f)
    });

    vertices.push_back({
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec2(1.0f,  1.0f)
    });

    vertices.push_back({
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec2(1.0f,  1.0f)
    });

    vertices.push_back({
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec2(-1.0f,  1.0f)
    });

    indices =
    {
        1, 2, 6,
        6, 5, 1,
        0, 4, 7,
        7, 3, 0,
        4, 5, 6,
        6, 7, 4,
        0, 3, 2,
        2, 1, 0,
        0, 1, 5,
        5, 4, 0,
        3, 7, 6,
        6, 2, 3
    };

    return new Mesh(vertices, indices);
}




