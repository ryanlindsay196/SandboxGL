#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;

void main()
{
	FragColor = texture(albedoMap, TexCoord);// * vec4(ourColor, 1.0);
	//FragColor = texture(albedoMap, TexCoord) * texture(normalMap, TexCoord);
	//FragColor = texture(normalMap, TexCoord);
	FragColor = mix(texture(albedoMap, TexCoord), texture(normalMap, TexCoord), 0.0) * vec4(ourColor, 1);
}