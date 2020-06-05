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

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform vec3 lightPos;

uniform Material mat;
uniform Light light;

uniform vec3 viewPos;

void main()
{
	////obtain normal from normal map in range [0,1]
	//vec3 normal = texture(normalMap, fs_in.TexCoord).rgb;
	////transform normal vector to range [-1,1]
	//normal = normalize(normal * 2.0 - 1.0); // this normal is in tangent space
	//
	////get diffuse color
	//vec3 color = texture(albedoMap, fs_in.TexCoord).rgb;
	////ambient
	//vec3 ambient;
	//ambient = 0.4 * color;
	////diffuse
	//vec3 diffuse;
	//vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	//float diff = max(dot(lightDir, normal), 0.0);
	//diffuse = diff * color;
	////specular
	//vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	//vec3 reflectDir = reflect(-lightDir, normal);
	//vec3 halfwayDir = normalize(lightDir + viewDir);
	//float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	//
	//vec3 specular;
	//specular = vec3(0.2) * spec;
	//FragColor = vec4(ambient + diffuse + specular, 1.0);
	
	
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
	//lightDir = TBN * -normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	//float diff = max(dot(normal, lightDir), 0.0);
	//vec3 diffuse = light.diffuse * (diff * mat.diffuse);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(albedoMap, fs_in.TexCoord.xy)));
	//vec3 diffuse = (mat.diffuse);
	
	
	
	//specular
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	//viewDir = TBN * normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess * 128);
	vec3 specular = light.specular * (spec * mat.specular);
	specular = light.specular * (spec * vec3(texture(specularMap, fs_in.TexCoord).xyz));
	//specular = vec3(0.9, 0.4, 0.4);
	
	//float dist = length(lightPos - FragPos);
	//
	//float k0 = 1.0;
	//float k1 = 0.07;
	//float k2 = 0.032;
	//
	//float attenuation = 1.0/ (k0 / k1 * dist + k2 * (dist * dist));
	//attenuation *= 30;
	//ambient *= attenuation;
	//diffuse *= attenuation;
	//specular *= attenuation;
	
	//FragColor = vec4(vec3(ambient + diffuse + specular), 1.0) * texture(albedoMap, TexCoord.xy);
	FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);
	//FragColor = vec4(texture(specularMap, TexCoord.xy).xyz, 1) + vec4(texture(normalMap, TexCoord.xy).xyz, 1) * vec4(texture(albedoMap, TexCoord.xy).xyz, 1);
	//FragColor = vec4(vec3(texture(albedoMap, TexCoord.xy).xyz + diffuse + specular), 1.0);

	//float near = 0.1;
	//float far = 100.0;
	//float z = gl_FragCoord.z * 2.0 - 1.0;
	//float depth = 1 - (2.0 * near * far) / (far + near - z * (far - near));
	//FragColor = vec4(vec3(depth), 1.0);

}