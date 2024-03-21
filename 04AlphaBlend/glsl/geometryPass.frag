#version 420 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragWorldPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex;
uniform float roughness;
uniform float metallic;

void main()
{
    gPosition.rgb = FragWorldPos.rgb;
    gPosition.a = roughness;

    gNormal = normalize(Normal);

    gAlbedoSpec.rgb = texture(tex, TexCoord).rgb;
    gAlbedoSpec.a = metallic;
}