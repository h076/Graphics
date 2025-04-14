#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 col;
in vec3 nor;
in vec3 FragPosWorldSpace;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 camPos;
uniform vec3 lightPos;

float CalculateDirectionalIllumination();
float CalculatePositionalIllumination();
float CalculateSpotIllumination();

void main()
{
	float phong = CalculateSpotIllumination();

    fColour = vec4(phong * col * lightColour, 1.0);
}

float CalculateDirectionalIllumination()
{
    float ambient = 0.1;
    
    // 2. Diffuse contribution
    vec3 Nnor = normalize(nor);
    // For a directional light, the light vector is the negative of lightDirection.
    vec3 Nto_light = normalize(-lightDirection);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);
    
    // 3. Specular contribution
    // Use the non-negated light direction and compute the reflection vector.
    vec3 Nfrom_light = normalize(lightDirection);
    vec3 NrefLight = reflect(Nfrom_light, Nnor);
    // Compute view direction (from fragment to camera)
    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);
    float specular = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0);
    
    // 4. Sum all terms to obtain the phong intensity (a scalar value)
    float phong = ambient + diffuse + specular;
    
    return phong;
}

float CalculatePositionalIllumination()
{
    // 1. Ambient (same as directional light)
    float ambient = 0.1;
    
    // 2. Diffuse
    vec3 Nnor = normalize(nor);
    // For a positional light, compute light direction from the fragment to the light position.
    vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);
    
    // 3. Specular
    // For a positional light, define NFromLight as the negative of Nto_light.
    vec3 Nfrom_light = -Nto_light;
    // Compute reflection of the incoming light about the normal.
    vec3 NrefLight = reflect(Nfrom_light, Nnor);
    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);
    float specular = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0);
    
    // 4. Attenuation
    float distance = length(lightPos - FragPosWorldSpace);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.002 * distance * distance);
    
    // 5. Combine the components using attenuation; the final phong is a scalar value.
    float phong = (ambient + diffuse + specular) * attenuation;
    
    return phong;
}

float CalculateSpotIllumination()
{
    float ambient = 0.1;

    // 2. Diffuse: compute the normalized vector from fragment to light position.
    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);

    // 3. Specular: for a positional light, NFromLight is the opposite of Nto_light.
    vec3 Nfrom_light = -Nto_light;
    vec3 NrefLight = reflect(Nfrom_light, Nnor);
    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);
    float specular = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0);

    // 4. Attenuation: compute distance-based attenuation.
    float distance = length(lightPos - FragPosWorldSpace);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.002 * distance * distance);

    // 5. Spotlight cone factor:
    // Convert the cutoff angle (15.0 degrees) to radians and compute its cosine.
    float phi = cos(radians(15.0));
    // Compute the spot light's direction (assumed to be given by the uniform lightDirection)
    vec3 NSpotDir = normalize(lightDirection);
    // Calculate theta: dot product of the incoming light direction (Nfrom_light) and the spot direction.
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