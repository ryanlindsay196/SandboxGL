#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
//layout (location = 3) in vec3 aTangent;
//layout (location = 4) in vec3 aBitangent;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;
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
	ourColor = vec3(1, 1, 1);
	//ourColor.z = aTexCoord.x;
	TexCoord = aTexCoord;
	Normal = aNormal;
}