#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 fragColor;

uniform mat4 modelTransform;

void main(void) 
{
	gl_Position = modelTransform * vec4(position, 1.0);

	fragColor = color;
}