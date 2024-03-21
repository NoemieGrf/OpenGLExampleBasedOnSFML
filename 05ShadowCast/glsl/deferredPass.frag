#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

uniform Light lights[5];
uniform vec3 viewPos;

void main()
{
    vec3 fragWorldPosition = texture(gPosition, TexCoords).rgb;
    float roughness = texture(gPosition, TexCoords).a;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float metallic = texture(gAlbedoSpec, TexCoords).a;
    vec3 viewDir  = normalize(viewPos - fragWorldPosition);
    
    vec3 lighting  = albedo * 0.3; 
    for(int i = 0; i < 5; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - fragWorldPosition);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].Color * roughness;
        
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        vec3 specular = pow(max(dot(normal, halfwayDir), 0.0), 16.0) * lights[i].Color * metallic;
        
        // attenuation
        float distance = length(lights[i].Position - fragWorldPosition);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        
        lighting += (diffuse + specular) * attenuation;        
    }
    
    FragColor = vec4(lighting, 1.0);
}