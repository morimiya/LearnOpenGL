#version 330 core
in vec3 Normal;
in vec3 FragPos;

in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
}; 

uniform Material material;

struct Light {
	vec3 position;
    vec3 direction;
	float cutOff;
	float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
	// 环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	vec3 diffuse = vec3(0);
	vec3 specular = vec3(0);

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 

	if(theta > light.outerCutOff) {
		// 漫反射光
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = intensity * light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

		// 镜面光
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		specular = intensity * light.specular * spec * vec3(texture(material.specular, TexCoord));
	}

    vec3 result = ambient  + diffuse + specular;
    FragColor = vec4(result, 1.0);
}