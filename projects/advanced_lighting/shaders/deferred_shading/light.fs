#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPostion;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 Position;
	vec3 Color;

	float Linear;
	float Quadratic;
};

uniform Light lights[32];
uniform vec3 viewPos;

void main()
{
	vec3 FragPos = texture(gPostion, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	vec3 lighting = Diffuse * 0.1;
	vec3 viewDir = normalize(viewPos - FragPos);
	for (int i = 0; i < 32; ++i) {
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
		vec3 specular = lights[i].Color * spec * Specular;

		float distance = length(lights[i].Position - FragPos);
		float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);

		lighting += attenuation * diffuse + attenuation * specular;
	}
	FragColor = vec4(lighting, 1.0);
	// FragColor = vec4(FragPos, 1.0);
}