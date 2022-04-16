#version 330 core

// Light count
#define DirectionalLightCount 1
#define PointLightCount 9

in VS_OUT 
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;
layout (location = 2) out vec4 FragDepth;

struct DirectionalLight 
{
    vec3 direction;
    vec3 color;

    float intensity;
};  


struct PointLight 
{
    vec3 position;
    vec3 color;

    float intensity;
    float range;
};  

uniform DirectionalLight directionalLights[DirectionalLightCount];
uniform PointLight pointLights[PointLightCount];

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;

// Shadows
uniform sampler2D shadowMap;

uniform float shininess;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

float ShadowCalculation(vec4 fragPosLight)
{
    // perform perspective divide
    vec3 projCoords = fs_in.fragPosLightSpace.xyz / fs_in.fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(-directionalLights[0].direction);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 ComputeDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffuseColor, float specularStrength, float shadow)
{
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec3 ambient = 0.15 * light.color;

    // Diffuse
    float diffuse = max(dot(norm, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    return (ambient + (1 - shadow) * diffuse * diffuseColor + specular * specularStrength) * light.intensity * light.color; 
}

vec3 ComputePointLight(PointLight light, vec3 norm, vec3 viewDir, vec3 diffuseColor, float specularStrength)
{	
    vec3 lightDir = normalize(light.position - fs_in.position);

    // No ambient for now

    // Diffuse
    float diffuse = max(dot(norm, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Attenuation
    float distance = length(light.position - fs_in.position);
    float attenuation = clamp(1.0f - (distance * distance / (light.range * light.range)), 0.0, 1.0); 

	return (diffuse * diffuseColor + specular * specularStrength) * attenuation * light.intensity * light.color;
}

float near = 0.1; 
float far  = 10.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{   
    // Calculate common variables
    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(cameraPosition - fs_in.position);

    vec3 diffuseColor = texture(albedoMap, fs_in.texCoords).rgb;
    float specularStrength = texture(roughnessMap, fs_in.texCoords).r; // using roughness as specular

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.fragPosLightSpace);  

    // Directional lighting
    vec3 result = ComputeDirectionalLight(directionalLights[0], norm, viewDir, diffuseColor, specularStrength, shadow);

    // Point lights
    for(int i = 0; i < PointLightCount; i++)
	{
		result += ComputePointLight(pointLights[i], norm, viewDir, diffuseColor, specularStrength);    
	}

    // Spot lights
    //result += SpotLight(spotLight, norm, viewDir);    

    // Default
    FragColor = vec4(result, 1.0);
    FragNormal = vec4(norm, 1.0);
    FragDepth = vec4(result, 1.0);// vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1.0);

    // Environment
    //vec3 I = normalize(position - cameraPosition);
    //vec3 R = reflect(I, normalize(normal));
    ///FragColor = vec4(texture(skybox, R).rgb, 1.0);
}


