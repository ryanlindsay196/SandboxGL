#version 330 core
out vec4 FragColor;
in VS_OUT {
	vec3 Color;
	vec3 Normal;
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Bitangent;
	vec3 FragPos;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

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

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS X
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform vec3 lightPos;

uniform Material mat;
uniform Light light;

uniform vec3 viewPos;

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	//ambient
	//vec3 ambient = mat.ambient;
	vec3 ambient = light.ambient * vec3(texture(albedoMap, fs_in.TexCoord.xy).xyz);
	
	//obtain normal from normal map in range [0,1]
	vec3 normal = texture(normalMap, fs_in.TexCoord.xy).rgb;
	//transform normal vector to range [-1, 1]
	normal = normalize(normal * 2.0 - 1.0);
	//normal = normalize(TBN * normal);
	
	//diffuse
	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = -normalize(light.position - fs_in.FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(normalMap, fs_in.TexCoord.xy)));
		
	//specular
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess * 128);
	vec3 specular = light.specular * (spec * mat.specular);
	specular = light.specular * (spec * vec3(texture(specularMap, fs_in.TexCoord).xyz));
	
	FragColor = vec4(vec3(ambient + diffuse + specular), 1.0) * texture(albedoMap, fs_in.TexCoord.xy);
	//FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);

	vec3 result = vec3(0,0,0);
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);

	FragColor = vec4(result, 1.0);
	//FragColor = texture(albedoMap, fs_in.TexCoord.xy);
	//FragColor = vec4(specular, 0);
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(pointLight.position - fragPos);
	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	//attenuation
	float distance = length(pointLight.position - fragPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + 
								pointLight.quadratic * (distance * distance));

	//combine results
	vec3 ambient = pointLight.ambient * vec3(texture(albedoMap, fs_in.TexCoord));
	vec3 diffuse = pointLight.diffuse * diff * vec3(texture(albedoMap, fs_in.TexCoord));
	vec3 specular = pointLight.specular * spec * vec3(texture(specularMap, fs_in.TexCoord));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}