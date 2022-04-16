#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out VS_OUT 
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    // Set world space position
    vs_out.position = vec3(model * vec4(aPos, 1.0));

    // Set world space normal
    vs_out.normal = transpose(inverse(mat3(model))) * aNormal;

    // Set tex coords
    vs_out.texCoords = aTexCoords;

    // Light spcae
    vs_out.fragPosLightSpace = lightSpaceMatrix * vec4(vs_out.position, 1.0);

    // Move to screen space 
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}