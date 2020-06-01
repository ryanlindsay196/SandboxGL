#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform vec3 lightPos;

void main()
{
	//vec3 tempLightPos = vec3(lightPos);
	//vec3 tempLightPos = vec3(20, 0.4, 0);
	float ambientStrength = 0.1;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	
	FragColor = (diff + ambientStrength) * mix(texture(albedoMap, TexCoord), texture(normalMap, TexCoord), 0.0) * vec4(ourColor, 1);
	FragColor = (diff + ambientStrength) * texture(albedoMap, TexCoord) * vec4(ourColor, 1);
	FragColor = texture(albedoMap, TexCoord);// * vec4(ourColor, 1);
	//FragColor = vec4(tempLightPos, 1.0);
	//FragColor = vec4(diff, diff, diff, 1.0);
	//FragColor = texture(albedoMap, TexCoord);// * vec4(ourColor, 1.0);
	//FragColor = texture(albedoMap, TexCoord) * texture(normalMap, TexCoord);
	//FragColor = texture(normalMap, TexCoord);
	//FragColor = vec4((FragColor.x + FragColor.y + FragColor.z) / 3, (FragColor.x + FragColor.y + FragColor.z) / 3, (FragColor.x + FragColor.y + FragColor.z) / 3, 1);
}