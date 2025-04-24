# version 450 core

layout (location = 0) out vec4 fColour;

in vec3 col;
in vec3 nor;
in vec3 FragPosWorldSpace;

uniform vec3 spotDirection;
uniform vec3 lightColour;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform float cutOffAngle;

void main()
{
	float ambient = 0.1;

	// Calculate diffuse
	vec3 Nnor = normalize(nor);
	vec3 Nto_light = normalize(spotDirection - FragPosWorldSpace);
	float diffuse = max(dot(Nnor, Nto_light), 0.0);

	// Calculate specular
	vec3 NFromLight = -NToLight;
	vec3 NrefLight = reflect(Nfrom_light, Nnor);
	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 NcamDirection = normalize(camDirection);
	float specular = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0);

	// Calculate attenuation
	float distance = length(lightPos - FragPosWorldSpace);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.002 * distance * distance);

	// Spot light cone factor
	float phi = cos(radians(cutOffAngle)); // cut off angle
	vec3 NSpotDir = normalize(spotDirection);
	float theta = dot(NFromLight, NSpotDir);

	if (theta > phi)
    {
        // Inside the spotlight cone: add ambient, diffuse, specular then apply attenuation.
        phong = (ambient + diffuse + specular) * attenuation;
    }
    else
    {
        // Outside the cone: use only the ambient term modulated by attenuation.
        phong = ambient * attenuation;
    }

	return phong;
}