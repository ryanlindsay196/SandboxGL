#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 position;
uniform mat4 projection;
uniform mat4 view;
uniform vec2 size;

void main()
{
	TexCoords = vertex.zw;
	vec4 billboard = (view * position) * vec4(vec3(0.0), 1.0);
	vec4 newPosition = projection * billboard +
	vec4(vertex.xy * size, 0.0, 0.0);

	gl_Position = newPosition;
	//gl_Position = vec4(vertex.xy * size, -20.0, 1.0);
	//gl_Position = vec4(gl_InstanceID, 0, 0, 0);
}