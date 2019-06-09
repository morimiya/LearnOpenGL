#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform bool invertedNormals;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_height0;

void main()
{
	gPosition = FragPos;
	gNormal = normalize(Normal);
	if (invertedNormals) {
		gAlbedoSpec.rgb = vec3(0.95);
		gAlbedoSpec.a = 1.0;
	}
	else {
		gAlbedoSpec.rgb = texture(texture_diffuse0, TexCoords).rgb;
		gAlbedoSpec.a = texture(texture_specular0, TexCoords).r;
	}
}