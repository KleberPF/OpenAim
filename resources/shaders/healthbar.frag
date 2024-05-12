#version 330 core
out vec4 FragColor;

// These are the coordinates of the plane before matrix transformations
// The x value, which is what matters here, ranges from -0.5 to 0.5
in vec3 FragPos;

struct Material {
    vec3 color;
};

uniform float healthPercentage;
uniform Material material;

void main()
{
	// normalize the FragPos.x to be in [0, 1]
	float pos = FragPos.x + 0.5f;

	if (pos <= healthPercentage) {
		FragColor = vec4(material.color, 1.0f);
	} else {
		FragColor = vec4(vec3(0.0f), 1.0f);
	}
}