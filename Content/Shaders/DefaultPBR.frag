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

uniform vec3 camPos;

// PBR textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube specularMap;
uniform sampler2D BRDFLUT;
//uniform int totalMipLevels;

// Lighting
uniform PointLight pointLights[PointLightCount];
uniform DirectionalLight directionalLights[DirectionalLightCount];

const float PI = 3.14159265359;

// Trick to get tangent-normals to world-space
vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, fs_in.texCoords).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(fs_in.position);
    vec3 Q2 = dFdy(fs_in.position);
    vec2 st1 = dFdx(fs_in.texCoords);
    vec2 st2 = dFdy(fs_in.texCoords);

    vec3 N = normalize(fs_in.normal);
    vec3 T = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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
    // Sample each PBR texture
    vec3 albedo = pow(texture(albedoMap, fs_in.texCoords).rgb, vec3(2.2));
    float metallic = texture(metallicMap, fs_in.texCoords).r;
    float roughness = texture(roughnessMap, fs_in.texCoords).r;

    // Get normal and view vector
    vec3 N = GetNormalFromMap();
    vec3 V = normalize(camPos - fs_in.position);
    vec3 R = reflect(-V, N); 

    // Calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // Reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < PointLightCount; ++i) 
    {
        // Calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - fs_in.position);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - fs_in.position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * attenuation;        
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G = GeometrySmith(N, V, L, roughness);      
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	 
             
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);   
        
        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   

    for(int i = 0; i < DirectionalLightCount; ++i) 
    {
        // Use directional light direction
        vec3 L = normalize(-directionalLights[i].direction);
        vec3 H = normalize(V + L);
        
        // No attenuation
        vec3 radiance = directionalLights[i].color;    
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G = GeometrySmith(N, V, L, roughness);      
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	 
             
        // Scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);   
        
        // Add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }

    // Ambient lighting (IBL)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 specularColor = textureLod(specularMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = specularColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular);
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
    FragNormal = vec4(N, 1.0);
    FragDepth = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1.0);
    //FragColor = vec4(texture(irradianceMap, N).rgb, 1.0); // Debug irradiance
    //FragColor = vec4(textureLod(specularMap, R,  roughness * MAX_REFLECTION_LOD).rgb, 1.0); // debug specular
} 