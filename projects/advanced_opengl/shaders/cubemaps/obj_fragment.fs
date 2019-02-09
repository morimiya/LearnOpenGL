#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_height0;
uniform samplerCube skybox;

out vec4 FragColor;

void main()
{             
    // Diffuse
    vec3 diffuse_color = vec3(texture(texture_diffuse0, TexCoords));
    // Reflection
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 reflect_color = vec3(texture(skybox, R)) * vec3(texture(texture_height0, TexCoords));
    
	// Combine them
    FragColor = vec4(diffuse_color + reflect_color, 1.0);
	// FragColor = vec4(reflect_color, 1.0);
	// FragColor = vec4(reflect_color, 1.0);
	// FragColor = mix(textureCube(skybox, R), textureCube(skybox, R), 0.3);
	// FragColor = mix(texture2D(texture_diffuse0, TexCoords), texture2D(texture_diffuse0, TexCoords), 0.3);
}
