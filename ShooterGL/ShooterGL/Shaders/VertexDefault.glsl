#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 Color;
out vec2 TexCoord;
out vec3 Normal;
out vec3 Tangent;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform vec3 lightPos;

void main()
{
	//gl_Position = model * vec4(aPos, 1.0);
	//gl_Position = view * model * vec4(aPos, 1.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Color = vec3(1, 1, 1);
	TexCoord = aTexCoord;
	//Normal = aNormal;
	Normal = mat3(transpose(inverse(model))) * aNormal;
	Tangent = aTangent;
}