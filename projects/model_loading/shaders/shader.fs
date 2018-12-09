#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_diffuse5;
uniform sampler2D texture_diffuse6;
uniform sampler2D texture_diffuse7;
uniform sampler2D texture_diffuse8;

uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;
uniform sampler2D texture_specular5;
uniform sampler2D texture_specular6;
uniform sampler2D texture_specular7;
uniform sampler2D texture_specular8;

// 聚光
struct SpotLight {
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform SpotLight spotLight;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// FragColor = texture(texture_diffuse0, TexCoords);
	// 属性
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
 
	// 聚光
	vec3 result = CalcSpotLight(spotLight, norm, FragPos, viewDir);    

	FragColor = vec4(result, 1.0);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	// 环境光
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, TexCoords));
	vec3 diffuse = vec3(0);
	vec3 specular = vec3(0);

	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 

	if(theta > light.outerCutOff) {
		// 漫反射光
		float diff = max(dot(normal, lightDir), 0.0);
		diffuse = intensity * light.diffuse * diff * vec3(texture(texture_diffuse0, TexCoords));

		// 镜面光
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		specular = intensity * light.specular * spec * vec3(texture(texture_specular0, TexCoords));
	}

	float disition = length(light.position - fragPos);
	float attennuation = 1.0 / (light.constant + light.linear * disition + light.quadratic * disition * disition);

	return attennuation * (ambient  + diffuse + specular);
}