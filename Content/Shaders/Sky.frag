#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;
layout (location = 2) out vec4 FragDepth;

in vec3 position;

uniform samplerCube environmentMap;

void main()
{   
    vec3 envColor = texture(environmentMap, position).rgb;

    // HDR tonemap and gamma correct
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor,vec3(1.0/2.2));

    FragColor = vec4(envColor, 1.0);
    FragNormal = vec4(0, 0, 0, 1);
    FragDepth = vec4(0, 0, 0, 1);
}