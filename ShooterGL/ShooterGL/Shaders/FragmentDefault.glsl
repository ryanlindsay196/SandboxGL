#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
	vec3 ambient;
	vec3 specular;
	vec3 diffuse;
	float shininess;
};
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform vec3 lightPos;

uniform Material mat;
uniform Light light;

uniform vec3 viewPos;

void main()
{
	//ambient
	vec3 ambient = mat.ambient;
	//vec3 ambient = light.ambient * mat.ambient;

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = light.diffuse * (diff * mat.diffuse);
	vec3 diffuse = (mat.diffuse);

	//specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess * 128);
	vec3 specular = light.specular * (spec * mat.specular);
	//specular = vec3(0.9, 0.4, 0.4);
	
	FragColor = vec4(vec3(ambient + diffuse + specular), 1.0) * texture(albedoMap, TexCoord.xy);
	//float ambientStrength = 0.1;
	//vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(lightPos - FragPos);
	//float diff = max(dot(norm, lightDir), 0.0);
	//
	//FragColor = (diff + ambientStrength) * texture(albedoMap, TexCoord.xy) * vec4(Color, 1);
}