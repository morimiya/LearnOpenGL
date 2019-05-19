#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(scene, TexCoords).rgb;
	// vec3 hdrColor = vec3(0);
	if (bloom) {
		hdrColor += texture(bloomBlur, TexCoords).rgb;
	}
	// reinhard
	// vec3 result = hdrColor / (hdrColor + vec3(1.0));
	// exposure
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	result = pow(result, vec3(1.0 / gamma));
	FragColor = vec4(result, 1.0);
}