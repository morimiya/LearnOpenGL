#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;
uniform vec3 viewPos;

void main()
{           
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // left diffuse
	vec3 normal = normalize(fs_in.Normal);

	vec3 leftLightPos = vec3(-1.5f, 0.0f, 0.0f);
    vec3 leftLightDir = normalize(leftLightPos - fs_in.FragPos);
    float leftDiff = max(dot(leftLightDir, normal), 0.0);
    vec3 leftDiffuse = leftDiff * color;

	// right diffuse
	vec3 rightLightPos = vec3(1.5f, 0.0f, 0.0f);
    vec3 rightLightDir = normalize(rightLightPos - fs_in.FragPos);
    float rightDiff = max(dot(rightLightDir, normal), 0.0);
    vec3 rightDiffuse = rightDiff * color;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 diffuse;
    float spec = 0.0;

    if(gl_FragCoord.x < 400)
    {
		diffuse = leftDiffuse;
        vec3 reflectDir = reflect(-leftLightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    else
    {
		diffuse = rightDiffuse;
        vec3 halfwayDir = normalize(rightLightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    vec3 specular = vec3(0.3) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}