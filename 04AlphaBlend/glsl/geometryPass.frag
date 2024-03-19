#version 420 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = texture(tex, TexCoord).rgb;
    gAlbedoSpec.a = 0.8;
}