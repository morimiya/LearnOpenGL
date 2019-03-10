#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMtx;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMtx * model * vec4(aPos, 1.0);
}