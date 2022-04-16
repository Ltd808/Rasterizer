#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;
layout (location = 2) out vec4 FragDepth;

uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
    FragNormal = vec4(0, 0, 0, 1);
    FragDepth = vec4(0, 0, 0, 1);
}