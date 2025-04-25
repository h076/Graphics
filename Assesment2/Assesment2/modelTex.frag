#version 450 core

layout (location = 0) out vec4 fColour;

in vec2 TexCoord;
in vec3 nor;
in vec3 FragPosWorldSpace;

uniform sampler2D diffuseMap;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform float cutOffAngle;

float CalculateSpotIllumination();

void main() {
    float phong = CalculateSpotIllumination();
    vec4 tex = texture(diffuseMap, TexCoord);
    vec3 lit = phong * tex.rgb * lightColour;
    fColour = vec4(lit, tex.a);
}

float CalculateSpotIllumination()
{
    float ambient = 0.1;

    // Calculate diffuse
    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);

    // Calculate specular
	vec3 Nfrom_light = -Nto_light;
	vec3 NrefLight = reflect(Nfrom_light, Nnor);
	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 NcamDirection = normalize(camDirection);
	float specular = pow(max(dot(NcamDirection, NrefLight), 0.0), 64.0);

    // Calculate attenuation
	float distance = length(lightPos - FragPosWorldSpace);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.002 * distance * distance);

    // Spot light cone factor
	float phi = cos(radians(cutOffAngle));
	vec3 NSpotDir = normalize(lightDirection);
	float theta = dot(Nfrom_light, NSpotDir);

    // 6. Compute the final phong term based on whether the fragment is inside the spot cone.
    float phong;
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
