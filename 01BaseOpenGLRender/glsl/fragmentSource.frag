#version 420 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform float fragIn;
uniform sampler2D ourTexture;
uniform sampler2D ourFace;

void main()
{
    FragColor = mix(texture(ourTexture, TexCoord), texture(ourFace, TexCoord), 0.5);
	FragColor = mix(FragColor, vec4(Normal, 1.0), 0.5);
	FragColor = vec4(FragColor.x, FragColor.y * fragIn, FragColor.z, FragColor.w);
}