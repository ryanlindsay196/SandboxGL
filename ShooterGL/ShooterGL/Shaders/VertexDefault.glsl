#version 330 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2 ) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;

out VS_OUT {
	vec3 Color;
	vec3 Normal;
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Bitangent;
	vec3 FragPos;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 gBones[100];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

uniform vec3 viewPos;

void main()
{
	mat4 BoneTransform = gBones[BoneIDs.x] * Weights.x;
	BoneTransform += gBones[BoneIDs.y] * Weights.y;
	BoneTransform += gBones[BoneIDs.z] * Weights.z;
	BoneTransform += gBones[BoneIDs.w] * Weights.w;

	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Color = vec3(1, 1, 1);
	vs_out.TexCoord = aTexCoord;
	
	//Normal = aNormal;
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	
	vs_out.Tangent = aTangent;
	vs_out.Bitangent = aBitangent;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;


	gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_Position = projection * view * BoneTransform * model * vec4(aPos, 1.0);
	gl_Position = projection * view * model * BoneTransform * vec4(aPos, 1.0);
}