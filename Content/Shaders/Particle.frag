#version 430 core
layout (location = 0) out vec4 FragColor;

in vec2 texCoords;
in vec4 position;
in vec4 color;

//out vec4 color;

uniform sampler2D myTex;

void main()
{
    vec4 texColor = texture(myTex, texCoords);
    FragColor = color * texColor;
}  