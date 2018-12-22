#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
	// 让z值为w，使得在进行透视除法之后，获得的z值恒为1.0，从而让天空盒默认在最背后。
	gl_Position = gl_Position.xyww;
}