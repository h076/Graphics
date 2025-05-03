#version 450 core

#define NUM_LIGHTS 5
#define NUM_POS_LIGHTS 4

layout (location = 0) out vec4 fColour;

in vec2 TexCoord;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace[NUM_LIGHTS];
in vec4 col;

uniform sampler2D diffuseMap;

uniform sampler2D shadowMaps[NUM_LIGHTS];

uniform bool useTexture;

struct spotLight {
	vec3 Pos;
	vec3 Dir;
	vec3 Col;
	float CutOffAngle;
};

uniform spotLight uSpotLight;

struct posLight {
	vec3 Pos;
	vec3 Dir;
	vec3 Col;
};

uniform posLight uPosLights[NUM_POS_LIGHTS];

uniform vec3 camPos;

float CalculateSpotIllumination(spotLight sLight, sampler2D shadowMap, vec4 FragPosLS);
float CalculatePositionalIllumination(posLight pLight, sampler2D shadowMap, vec4 FragPosLS);

float shadowSample(sampler2D shadowMap, vec4 FragPosLS, vec3 lightDirection);

void main() {
    vec3 totalPhong = vec3(0.0);
    for(int l=0; l<NUM_POS_LIGHTS; l++) {
        totalPhong += CalculatePositionalIllumination(uPosLights[l], shadowMaps[l], FragPosProjectedLightSpace[l]);
    }
    totalPhong += CalculateSpotIllumination(uSpotLight, shadowMaps[NUM_POS_LIGHTS], FragPosProjectedLightSpace[NUM_POS_LIGHTS]);

    if(useTexture) {
        vec4 tex = texture(diffuseMap, TexCoord);
        vec3 lit = totalPhong * tex.rgb * uSpotLight.Col;
        fColour = vec4(lit, 1.f);
    }else {
        vec3 lit = totalPhong * col.rgb * uSpotLight.Col;
        fColour = vec4(lit, col.a);
    }

    //vec4 tex = texture(diffuseMap, TexCoord);
    //vec3 lit = totalPhong * tex.rgb * uSpotLight.Col;
    //fColour = vec4(lit, 1.f);
}

float CalculateSpotIllumination(spotLight sLight, sampler2D shadowMap, vec4 FragPosLS) {
	vec3 lightDirection = sLight.Dir;
	vec3 lightColour = sLight.Col;
	vec3 lightPos = sLight.Pos;
	float cutOffAngle = sLight.CutOffAngle;

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

    // get shadow for this light
    float shadow = shadowSample(shadowMap, FragPosLS, sLight.Dir);

    // 6. Compute the final phong term based on whether the fragment is inside the spot cone.
    float phong;
    if (theta > phi)
    {
        // Inside the spotlight cone: add ambient, diffuse, specular then apply attenuation.
        phong = (ambient + ((1.f - shadow)*(diffuse + specular))) * attenuation;
    }
    else
    {
        // Outside the cone: use only the ambient term modulated by attenuation.
        phong = ambient * attenuation;
    }
    
    return phong;
}

float CalculatePositionalIllumination(posLight pLight, sampler2D shadowMap, vec4 FragPosLS) {
    vec3 lightDirection = pLight.Dir;
	vec3 lightColour = pLight.Col;
	vec3 lightPos = pLight.Pos;

    // Ambient
    float ambient = 0.1;
    
    // Diffuse
    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);
    
    // Specular
    vec3 Nfrom_light = -Nto_light;
    vec3 NrefLight = reflect(Nfrom_light, Nnor);
    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);
    float specular = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0);
    
    // Attenuation
    float distance = length(lightPos - FragPosWorldSpace);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.002 * distance * distance);

    // get shadow for this light
    float shadow = shadowSample(shadowMap, FragPosLS, pLight.Dir);

    float phong = (ambient + ((1.f - shadow)*(diffuse + specular))) * attenuation;
    
    return phong;
}

float shadowSample(sampler2D shadowMap, vec4 FragPosLS, vec3 lightDirection) {
    // project from clip space to ndc
    vec3 ndc = FragPosLS.xyz / FragPosLS.w;
    vec3 ss = ndc * 0.5 + 0.5;

    // test if outside shadow map
    if (ss.x < 0.0 || ss.x > 1.0 || ss.y < 0.0 || ss.y > 1.0 || ss.z > 1.0)
        return 0.f;

    float fragDepth = ss.z;
    float litDepth = texture(shadowMap, ss.xy).r;

    vec3 Nnor = normalize(nor);
    vec3 NtoLight = normalize(-lightDirection);
    float bias = max(0.05 * (1.0 - dot(Nnor, NtoLight)), 0.005);

    float shadow = 0.f;
    shadow = fragDepth > (litDepth+bias) ? 1.0 : 0.0;

    if (fragDepth > 1)
        shadow = 0.f;

    return shadow;
}