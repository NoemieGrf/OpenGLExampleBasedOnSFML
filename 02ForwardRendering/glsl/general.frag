#version 420 core

out vec4 FragColor;

struct Material 
{
    float roughness;
    float matellic;
}; 

struct Light 
{
    vec3 position;
    vec3 color;
};

in vec3 FragPos;  
in vec3 vNormal;  
in vec2 vTexCoord;

uniform sampler2D tex;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.color * 0.2f;
  	
    // diffuse 
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * diff * material.roughness;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = light.color * spec * material.matellic;  
        
    vec3 result = ambient + diffuse + specular;

    FragColor = texture(tex, vTexCoord) * vec4(result, 1.0);
} 