#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 position;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    position = vec3(aPos.x, aPos.z, aPos.y);
    vec4 clipPos = projection * view * vec4(aPos, 1.0);

    // Set z to w so sky vertex is on far clip plane
    gl_Position = clipPos.xyww;
}    