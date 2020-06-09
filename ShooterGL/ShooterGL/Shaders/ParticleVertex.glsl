#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 position;
uniform mat4 projection;
uniform mat4 view;
uniform vec2 offset;
uniform vec4 color;
uniform vec2 size;

void main()
{
	float scale = 0.1f;
	TexCoords = vertex.zw;
	ParticleColor = color;
	//gl_Position = projection * vec4((vertex.xy * scale) + offset, 1.0);
	gl_Position = projection * view * position * vec4((vertex.xy * size), 0.0 , 1.0);
}